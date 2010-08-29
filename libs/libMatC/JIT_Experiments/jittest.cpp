#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/raw_ostream.h>

#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Basic/Builtins.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/FileManager.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/HeaderSearchOptions.h>
#include <clang/Frontend/PreprocessorOptions.h>
#include <clang/Frontend/FrontendOptions.h>
#include <clang/Frontend/Utils.h>
#include <clang/Basic/IdentifierTable.h>
//#include <clang/Parse/Action.h>
#include <clang/Parse/Parser.h>
#include "clang/Parse/ParseAST.h"
#include <llvm/Target/TargetSelect.h>
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/LLVMContext.h"

#include <fstream>

/*----------------------------------------------------------------------------------------------------------------*/
//===--- .cpp - LLVM Code Generation Frontend Action ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/CodeGen/BackendUtil.h"

#include "clang/CodeGen/ModuleBuilder.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Timer.h"

using namespace clang;
using namespace llvm;

namespace {
    class JITConsumer : public ASTConsumer {
        Diagnostic &Diags;
        BackendAction Action;
        const CodeGenOptions &CodeGenOpts;
        const TargetOptions &TargetOpts;
        llvm::raw_ostream *AsmOutStream;
        ASTContext *Context;
        Timer LLVMIRGeneration;
        llvm::OwningPtr<CodeGenerator> Gen;
        llvm::OwningPtr<llvm::Module> TheModule;

    public:
        JITConsumer(BackendAction action, Diagnostic &_Diags,
                const CodeGenOptions &compopts,
                const TargetOptions &targetopts, bool TimePasses,
                const std::string &infile, llvm::raw_ostream *OS,
                LLVMContext &C) :
                    Diags(_Diags),
                    Action(action),
                    CodeGenOpts(compopts),
                    TargetOpts(targetopts),
                    AsmOutStream(OS),
                    LLVMIRGeneration("LLVM IR Generation Time"),
                    Gen(CreateLLVMCodeGen(Diags, infile, compopts, C)) {
            llvm::TimePassesIsEnabled = TimePasses;
        }

        llvm::Module *takeModule() { return TheModule.take(); }

        virtual void Initialize(ASTContext &Ctx) {
            Context = &Ctx;

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.startTimer();

            Gen->Initialize(Ctx);

            TheModule.reset(Gen->GetModule());

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.stopTimer();
        }

        virtual void HandleTopLevelDecl(DeclGroupRef D) {
            PrettyStackTraceDecl CrashInfo(*D.begin(), SourceLocation(),
                    Context->getSourceManager(),
                    "LLVM IR generation of declaration");

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.startTimer();

            Gen->HandleTopLevelDecl(D);

            if (llvm::TimePassesIsEnabled)
                LLVMIRGeneration.stopTimer();
        }

        virtual void HandleTranslationUnit(ASTContext &C) {
            {
                PrettyStackTraceString CrashInfo("Per-file LLVM IR generation");
                if (llvm::TimePassesIsEnabled)
                    LLVMIRGeneration.startTimer();

                Gen->HandleTranslationUnit(C);

                if (llvm::TimePassesIsEnabled)
                    LLVMIRGeneration.stopTimer();
            }

            // Silently ignore if we weren't initialized for some reason.
            if (!TheModule)
                return;

            // Make sure IR generation is happy with the module. This is released by
            // the module provider.
            Module *M = Gen->ReleaseModule();
            if (!M) {
                // The module has been released by IR gen on failures, do not double
                // free.
                TheModule.take();
                return;
            }

            assert(TheModule.get() == M &&
                    "Unexpected module change during IR generation");

            // Install an inline asm handler so that diagnostics get printed through
            // our diagnostics hooks.
            LLVMContext &Ctx = TheModule->getContext();
            void *OldHandler = Ctx.getInlineAsmDiagnosticHandler();
            void *OldContext = Ctx.getInlineAsmDiagnosticContext();
            Ctx.setInlineAsmDiagnosticHandler((void*)(intptr_t)InlineAsmDiagHandler,
                    this);



            EmitBackendOutput(Diags, CodeGenOpts, TargetOpts,
                    TheModule.get(), Action, AsmOutStream);

            Ctx.setInlineAsmDiagnosticHandler(OldHandler, OldContext);
        }

        virtual void HandleTagDeclDefinition(TagDecl *D) {
            PrettyStackTraceDecl CrashInfo(D, SourceLocation(),
                    Context->getSourceManager(),
                    "LLVM IR generation of declaration");
            Gen->HandleTagDeclDefinition(D);
        }

        virtual void CompleteTentativeDefinition(VarDecl *D) {
            Gen->CompleteTentativeDefinition(D);
        }

        virtual void HandleVTable(CXXRecordDecl *RD, bool DefinitionRequired) {
            Gen->HandleVTable(RD, DefinitionRequired);
        }

        static void InlineAsmDiagHandler(const llvm::SMDiagnostic &SM,void *Context,
                unsigned LocCookie) {
            SourceLocation Loc = SourceLocation::getFromRawEncoding(LocCookie);
            ((JITConsumer*)Context)->InlineAsmDiagHandler2(SM, Loc);
        }

        void InlineAsmDiagHandler2(const llvm::SMDiagnostic &,
                SourceLocation LocCookie);

        virtual void ExecuteModule( void ){
            std::string errorstring;

            if( !Diags.hasErrorOccurred() ) {
                ExecutionEngine* ee = ExecutionEngine::createJIT(TheModule.get(), &errorstring);
                Function* fn = ee->FindFunctionNamed( "f_t" );
                std::vector<GenericValue> arg;
                GenericValue ret = ee->runFunction( fn, arg );
		ee->freeMachineCodeForFunction(fn);
            }
            else{
                printf("Error!!! !!!");
            }
        }
    };
}

/// ConvertBackendLocation - Convert a location in a temporary llvm::SourceMgr
/// buffer to be a valid FullSourceLoc.
static FullSourceLoc ConvertBackendLocation(const llvm::SMDiagnostic &D,
        SourceManager &CSM) {
    // Get both the clang and llvm source managers.  The location is relative to
    // a memory buffer that the LLVM Source Manager is handling, we need to add
    // a copy to the Clang source manager.
    const llvm::SourceMgr &LSM = *D.getSourceMgr();

    // We need to copy the underlying LLVM memory buffer because llvm::SourceMgr
    // already owns its one and clang::SourceManager wants to own its one.
    const MemoryBuffer *LBuf =
            LSM.getMemoryBuffer(LSM.FindBufferContainingLoc(D.getLoc()));

    // Create the copy and transfer ownership to clang::SourceManager.
    llvm::MemoryBuffer *CBuf =
            llvm::MemoryBuffer::getMemBufferCopy(LBuf->getBuffer(),
                    LBuf->getBufferIdentifier());
    FileID FID = CSM.createFileIDForMemBuffer(CBuf);

    // Translate the offset into the file.
    unsigned Offset = D.getLoc().getPointer()  - LBuf->getBufferStart();
    SourceLocation NewLoc =
            CSM.getLocForStartOfFile(FID).getFileLocWithOffset(Offset);
    return FullSourceLoc(NewLoc, CSM);
}

/// InlineAsmDiagHandler2 - This function is invoked when the backend hits an
/// error parsing inline asm.  The SMDiagnostic indicates the error relative to
/// the temporary memory buffer that the inline asm parser has set up.
void JITConsumer::InlineAsmDiagHandler2(const llvm::SMDiagnostic &D,
        SourceLocation LocCookie) {
    // There are a couple of different kinds of errors we could get here.  First,
    // we re-format the SMDiagnostic in terms of a clang diagnostic.

    // Strip "error: " off the start of the message string.
    llvm::StringRef Message = D.getMessage();
    if (Message.startswith("error: "))
        Message = Message.substr(7);

    // If the SMDiagnostic has an inline asm source location, translate it.
    FullSourceLoc Loc;
    if (D.getLoc() != SMLoc())
        Loc = ConvertBackendLocation(D, Context->getSourceManager());

    // If this problem has clang-level source location information, report the
    // issue as being an error in the source with a note showing the instantiated
    // code.
    if (LocCookie.isValid()) {
        Diags.Report(FullSourceLoc(LocCookie, Context->getSourceManager()),
                diag::err_fe_inline_asm).AddString(Message);

        if (D.getLoc().isValid())
            Diags.Report(Loc, diag::note_fe_inline_asm_here);

        return;
    }

    // Otherwise, report the backend error as occuring in the generated .s file.
    // If Loc is invalid, we still need to report the error, it just gets no
    // location info.
    Diags.Report(Loc, diag::err_fe_inline_asm).AddString(Message);
}

//

CodeGenAction::CodeGenAction(unsigned _Act) : Act(_Act) {}

CodeGenAction::~CodeGenAction() {}

bool CodeGenAction::hasIRSupport() const { return true; }

void CodeGenAction::EndSourceFileAction() {
    // If the consumer creation failed, do nothing.
    if (!getCompilerInstance().hasASTConsumer())
        return;

    // Steal the module from the consumer.
    JITConsumer *Consumer = static_cast<JITConsumer*>(
            &getCompilerInstance().getASTConsumer());

    TheModule.reset(Consumer->takeModule());
}

llvm::Module *CodeGenAction::takeModule() {
    return TheModule.take();
}

static raw_ostream *GetOutputStream(CompilerInstance &CI,
        llvm::StringRef InFile,
        BackendAction Action) {

    switch (Action) {
        case Backend_EmitAssembly:
            return CI.createDefaultOutputFile(false, InFile, "s");

        case Backend_EmitLL:
            return CI.createDefaultOutputFile(false, InFile, "ll");

        case Backend_EmitBC:
            return CI.createDefaultOutputFile(true, InFile, "bc");

        case Backend_EmitNothing:
            return 0;
        case Backend_EmitMCNull:

        case Backend_EmitObj:
            return CI.createDefaultOutputFile(true, InFile, "o");

    }

    assert(0 && "Invalid action!");
    return 0;
}

ASTConsumer *CodeGenAction::CreateASTConsumer(CompilerInstance &CI,
        llvm::StringRef InFile) {
    BackendAction BA = static_cast<BackendAction>(Act);
    llvm::OwningPtr<llvm::raw_ostream> OS(GetOutputStream(CI, InFile, BA));
    if (BA != Backend_EmitNothing && !OS)
        return 0;

    return new JITConsumer(BA, CI.getDiagnostics(),
            CI.getCodeGenOpts(), CI.getTargetOpts(),
            CI.getFrontendOpts().ShowTimers, InFile, OS.take(),
            CI.getLLVMContext());
}

void CodeGenAction::ExecuteAction() {
    // If this is an IR file, we have to treat it specially.
    if (getCurrentFileKind() == IK_LLVM_IR) {
        BackendAction BA = static_cast<BackendAction>(Act);
        CompilerInstance &CI = getCompilerInstance();
        raw_ostream *OS = GetOutputStream(CI, getCurrentFile(), BA);
        if (BA != Backend_EmitNothing && !OS)
            return;

        bool Invalid;
        SourceManager &SM = CI.getSourceManager();
        const llvm::MemoryBuffer *MainFile = SM.getBuffer(SM.getMainFileID(),
                &Invalid);
        if (Invalid)
            return;

        // FIXME: This is stupid, IRReader shouldn't take ownership.
        llvm::MemoryBuffer *MainFileCopy =
                llvm::MemoryBuffer::getMemBufferCopy(MainFile->getBuffer(),
                        getCurrentFile().c_str());

        llvm::SMDiagnostic Err;
        TheModule.reset(ParseIR(MainFileCopy, Err, CI.getLLVMContext()));
        if (!TheModule) {
            // Translate from the diagnostic info to the SourceManager location.
            SourceLocation Loc = SM.getLocation(
                    SM.getFileEntryForID(SM.getMainFileID()), Err.getLineNo(),
                    Err.getColumnNo() + 1);

            // Get a custom diagnostic for the error. We strip off a leading
            // diagnostic code if there is one.
            llvm::StringRef Msg = Err.getMessage();
            if (Msg.startswith("error: "))
                Msg = Msg.substr(7);
            unsigned DiagID = CI.getDiagnostics().getCustomDiagID(Diagnostic::Error,
                    Msg);

            CI.getDiagnostics().Report(FullSourceLoc(Loc, SM), DiagID);
            return;
        }

        EmitBackendOutput(CI.getDiagnostics(), CI.getCodeGenOpts(),
                CI.getTargetOpts(), TheModule.get(),
                BA, OS);
        return;
    }

    // Otherwise follow the normal AST path.
    this->ASTFrontendAction::ExecuteAction();
}

//
EmitAssemblyAction::EmitAssemblyAction()
: CodeGenAction(Backend_EmitAssembly) {}

EmitBCAction::EmitBCAction() : CodeGenAction(Backend_EmitBC) {}

EmitLLVMAction::EmitLLVMAction() : CodeGenAction(Backend_EmitLL) {}

EmitLLVMOnlyAction::EmitLLVMOnlyAction() : CodeGenAction(Backend_EmitNothing) {}

EmitCodeGenOnlyAction::EmitCodeGenOnlyAction() : CodeGenAction(Backend_EmitMCNull) {}

EmitObjAction::EmitObjAction() : CodeGenAction(Backend_EmitObj) {}


//
JITConsumer *CreateJITConsumer(BackendAction Action,
        Diagnostic &Diags,
        const CodeGenOptions &CodeGenOpts,
        const TargetOptions &TargetOpts,
        bool TimePasses,
        const std::string& InFile,
        llvm::raw_ostream* OS,
        LLVMContext& C) {
    return new JITConsumer(Action, Diags, CodeGenOpts,
            TargetOpts, TimePasses, InFile, OS, C);
}

//
int main( void )
{
    InitializeNativeTarget();

    //std::filebuf fb;
    //fb.open("t.err",std::ios::out);
    //std::ostream ostr( &fb );
    //llvm::raw_os_ostream ost( ostr );
    std::string ErrorInfo1;
    llvm::raw_ostream * ost = new llvm::raw_fd_ostream("t.err",ErrorInfo1);

    //llvm::raw_stderr_ostream ost;

    DiagnosticOptions diag_opts;
    diag_opts.BinaryOutput = 0;
    diag_opts.ShowCarets = 1;

    TextDiagnosticPrinter *tdp = new TextDiagnosticPrinter(*ost, diag_opts);
    Diagnostic diag(tdp);
    LangOptions lang;
    lang.CPlusPlus = 1;
    lang.C99 = 1;
    lang.CPlusPlus0x = 0;
    lang.GNUKeywords = 1;
    lang.GNUMode = 1;
    lang.Microsoft = 0;
    lang.Bool = 1;
    lang.RTTI = 1;
    //lang.AccessControl = 1;
    lang.ShortWChar = 1;
    lang.Exceptions = 1;
    lang.MathErrno = 1;
    //lang.HeinousExtensions = 1;

    SourceManager sm( diag );
    FileManager fm;

    HeaderSearch headers(fm);


    TargetOptions target_opts;
    target_opts.Triple = LLVM_HOSTTRIPLE;
    
    //target_opts.CXXABI = "microsoft";

    TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, target_opts);
    Preprocessor pp(diag, lang, *ti, sm, headers);

    PreprocessorOptions ppio;
    HeaderSearchOptions hsopt;

    QFile f("header_paths.txt");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&f);
    QString path;
    while (!in.atEnd()) {
        path = in.readLine();
	
        hsopt.AddPath(path.toStdString(), clang::frontend::System, false, false, false );
    }
    f.close();


    FrontendOptions feopt;
    InitializePreprocessor(pp, ppio, hsopt, feopt );
    pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOptions().NoBuiltin);
    
    tdp->BeginSourceFile( lang, &pp );

    f.setFileName("sources.txt");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&f);
    path = in.readLine();

    const FileEntry *file = fm.getFile(path.toStdString());
    sm.createMainFileID(file);

    while (!in.atEnd()) {
        path = in.readLine();
        file = fm.getFile(path.toStdString());
        hsopt.AddPath(path.toStdString(), clang::frontend::System, false, false, false );
        sm.createFileID( file, SourceLocation(), SrcMgr::C_User );
    }
    f.close();

    //pp.EnterMainSourceFile();


    //IdentifierTable tab(lang);
    //MyAction action(pp);
    //Parser p(pp, action);
    //p.ParseTranslationUnit();
    //tab.PrintStats();

    //SelectorTable sel;
    
    CodeGenOptions codeGenOpts;
    codeGenOpts.DebugInfo = 0;
    codeGenOpts.OptimizationLevel = 3; //TODO: change to 4

    std::string ErrorInfo;
    llvm::raw_ostream* os = new llvm::raw_fd_ostream("t.a",ErrorInfo);

    LLVMContext llvmContext;
    
    JITConsumer *c = CreateJITConsumer(Backend_EmitLL, diag,
            codeGenOpts, target_opts,
            true, "mymodule",
            os, llvmContext);

    ASTContext ctx(pp.getLangOptions(), pp.getSourceManager(), pp.getTargetInfo(), pp.getIdentifierTable(), pp.getSelectorTable(), pp.getBuiltinInfo(), 0);

    ParseAST(pp, c, ctx, true, true);

    c->ExecuteModule();

    delete c;
    
    tdp->EndSourceFile();

    //Token Tok;

    //do {
    //	pp.Lex(Tok);                                                    
    //	if(diag.hasErrorOccurred())
    //		break;
    //	pp.DumpToken(Tok);
    //	std::cerr << std::endl;
    //} while(Tok.isNot(tok::eof));
    //

    return 0;
}
