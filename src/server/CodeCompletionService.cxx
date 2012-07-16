
#include <map>

#include <clang-c/Index.h>

#include <QTime>

#include "clangLib/ClangString.h"

#include "ClangTranslationUnit.h"
#include "CodeCompletionService.h"
#include "TranslationUnitManager.h"

// debugging
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>

namespace
{
   typedef std::map<CXCursorKind, ccs::CursorKind::type> CursorKindMap;
   
   CursorKindMap initCursorKindMap()
   {
      CursorKindMap rtn;
      
      // declarations
      rtn[CXCursor_UnexposedDecl] = ccs::CursorKind::UnexposedDecl;
      rtn[CXCursor_StructDecl] = ccs::CursorKind::StructDecl;
      rtn[CXCursor_UnionDecl] = ccs::CursorKind::UnionDecl;
      rtn[CXCursor_ClassDecl] = ccs::CursorKind::ClassDecl;
      rtn[CXCursor_EnumDecl] = ccs::CursorKind::EnumDecl;
      rtn[CXCursor_FieldDecl] = ccs::CursorKind::FieldDecl;
      rtn[CXCursor_EnumConstantDecl] = ccs::CursorKind::EnumConstantDecl;
      rtn[CXCursor_FunctionDecl] = ccs::CursorKind::FunctionDecl;
      rtn[CXCursor_VarDecl] = ccs::CursorKind::VarDecl;
      rtn[CXCursor_ParmDecl] = ccs::CursorKind::ParmDecl;
      rtn[CXCursor_ObjCInterfaceDecl] = ccs::CursorKind::ObjCInterfaceDecl;
      rtn[CXCursor_ObjCCategoryDecl] = ccs::CursorKind::ObjCCategoryDecl;
      rtn[CXCursor_ObjCProtocolDecl] = ccs::CursorKind::ObjCProtocolDecl;
      rtn[CXCursor_ObjCPropertyDecl] = ccs::CursorKind::ObjCPropertyDecl;
      rtn[CXCursor_ObjCIvarDecl] = ccs::CursorKind::ObjCIvarDecl;
      rtn[CXCursor_ObjCInstanceMethodDecl] = ccs::CursorKind::ObjCInstanceMethodDecl;
      rtn[CXCursor_ObjCClassMethodDecl] = ccs::CursorKind::ObjCClassMethodDecl;
      rtn[CXCursor_ObjCImplementationDecl] = ccs::CursorKind::ObjCImplementationDecl;
      rtn[CXCursor_ObjCCategoryImplDecl] = ccs::CursorKind::ObjCCategoryImplDecl;
      rtn[CXCursor_TypedefDecl] = ccs::CursorKind::TypedefDecl;
      rtn[CXCursor_CXXMethod] = ccs::CursorKind::CXXMethod;
      rtn[CXCursor_Namespace] = ccs::CursorKind::Namespace;
      rtn[CXCursor_LinkageSpec] = ccs::CursorKind::LinkageSpec;
      rtn[CXCursor_Constructor] = ccs::CursorKind::Constructor;
      rtn[CXCursor_Destructor] = ccs::CursorKind::Destructor;
      rtn[CXCursor_ConversionFunction] = ccs::CursorKind::ConversionFunction;
      rtn[CXCursor_TemplateTypeParameter] = ccs::CursorKind::TemplateTypeParameter;
      rtn[CXCursor_NonTypeTemplateParameter] = ccs::CursorKind::NonTypeTemplateParameter;
      rtn[CXCursor_TemplateTemplateParameter] = ccs::CursorKind::TemplateTemplateParameter;
      rtn[CXCursor_FunctionTemplate] = ccs::CursorKind::FunctionTemplate;
      rtn[CXCursor_ClassTemplate] = ccs::CursorKind::ClassTemplate;
      rtn[CXCursor_ClassTemplatePartialSpecialization] = ccs::CursorKind::ClassTemplatePartialSpecialization;
      rtn[CXCursor_NamespaceAlias] = ccs::CursorKind::NamespaceAlias;
      rtn[CXCursor_UsingDirective] = ccs::CursorKind::UsingDirective;
      rtn[CXCursor_UsingDeclaration] = ccs::CursorKind::UsingDeclaration;
      rtn[CXCursor_TypeAliasDecl] = ccs::CursorKind::TypeAliasDecl;
      rtn[CXCursor_ObjCSynthesizeDecl] = ccs::CursorKind::ObjCSynthesizeDecl;
      rtn[CXCursor_ObjCDynamicDecl] = ccs::CursorKind::ObjCDynamicDecl;
      rtn[CXCursor_CXXAccessSpecifier] = ccs::CursorKind::CXXAccessSpecifier;
      
      // references
      rtn[CXCursor_ObjCSuperClassRef] = ccs::CursorKind::ObjCSuperClassRef;
      rtn[CXCursor_ObjCProtocolRef] = ccs::CursorKind::ObjCProtocolRef;
      rtn[CXCursor_ObjCClassRef] = ccs::CursorKind::ObjCClassRef;
      rtn[CXCursor_TypeRef] = ccs::CursorKind::TypeRef;
      rtn[CXCursor_CXXBaseSpecifier] = ccs::CursorKind::CXXBaseSpecifier;
      rtn[CXCursor_TemplateRef] = ccs::CursorKind::TemplateRef;
      rtn[CXCursor_NamespaceRef] = ccs::CursorKind::NamespaceRef;
      rtn[CXCursor_MemberRef] = ccs::CursorKind::MemberRef;
      rtn[CXCursor_LabelRef] = ccs::CursorKind::LabelRef;
      rtn[CXCursor_OverloadedDeclRef] = ccs::CursorKind::OverloadedDeclRef;
      rtn[CXCursor_VariableRef] = ccs::CursorKind::VariableRef;
      
      // error conditions
      rtn[CXCursor_InvalidFile] = ccs::CursorKind::InvalidFile;
      rtn[CXCursor_NoDeclFound] = ccs::CursorKind::NoDeclFound;
      rtn[CXCursor_NotImplemented] = ccs::CursorKind::NotImplemented;
      rtn[CXCursor_InvalidCode] = ccs::CursorKind::InvalidCode;
      
      // expressions
      rtn[CXCursor_UnexposedExpr] = ccs::CursorKind::UnexposedExpr;
      rtn[CXCursor_DeclRefExpr] = ccs::CursorKind::DeclRefExpr;
      rtn[CXCursor_MemberRefExpr] = ccs::CursorKind::MemberRefExpr;
      rtn[CXCursor_CallExpr] = ccs::CursorKind::CallExpr;
      rtn[CXCursor_ObjCMessageExpr] = ccs::CursorKind::ObjCMessageExpr;
      rtn[CXCursor_BlockExpr] = ccs::CursorKind::BlockExpr;
      rtn[CXCursor_IntegerLiteral] = ccs::CursorKind::IntegerLiteral;
      rtn[CXCursor_FloatingLiteral] = ccs::CursorKind::FloatingLiteral;
      rtn[CXCursor_ImaginaryLiteral] = ccs::CursorKind::ImaginaryLiteral;
      rtn[CXCursor_StringLiteral] = ccs::CursorKind::StringLiteral;
      rtn[CXCursor_CharacterLiteral] = ccs::CursorKind::CharacterLiteral;
      rtn[CXCursor_ParenExpr] = ccs::CursorKind::ParenExpr;
      rtn[CXCursor_UnaryOperator] = ccs::CursorKind::UnaryOperator;
      rtn[CXCursor_ArraySubscriptExpr] = ccs::CursorKind::ArraySubscriptExpr;
      rtn[CXCursor_BinaryOperator] = ccs::CursorKind::BinaryOperator;
      rtn[CXCursor_CompoundAssignOperator] = ccs::CursorKind::CompoundAssignOperator;
      rtn[CXCursor_ConditionalOperator] = ccs::CursorKind::ConditionalOperator;
      rtn[CXCursor_CStyleCastExpr] = ccs::CursorKind::CStyleCastExpr;
      rtn[CXCursor_CompoundLiteralExpr] = ccs::CursorKind::CompoundLiteralExpr;
      rtn[CXCursor_InitListExpr] = ccs::CursorKind::InitListExpr;
      rtn[CXCursor_AddrLabelExpr] = ccs::CursorKind::AddrLabelExpr;
      rtn[CXCursor_StmtExpr] = ccs::CursorKind::StmtExpr;
      rtn[CXCursor_GenericSelectionExpr] = ccs::CursorKind::GenericSelectionExpr;
      rtn[CXCursor_GNUNullExpr] = ccs::CursorKind::GNUNullExpr;
      rtn[CXCursor_CXXStaticCastExpr] = ccs::CursorKind::CXXStaticCastExpr;
      rtn[CXCursor_CXXDynamicCastExpr] = ccs::CursorKind::CXXDynamicCastExpr;
      rtn[CXCursor_CXXReinterpretCastExpr] = ccs::CursorKind::CXXReinterpretCastExpr;
      rtn[CXCursor_CXXConstCastExpr] = ccs::CursorKind::CXXConstCastExpr;
      rtn[CXCursor_CXXFunctionalCastExpr] = ccs::CursorKind::CXXFunctionalCastExpr;
      rtn[CXCursor_CXXBoolLiteralExpr] = ccs::CursorKind::CXXBoolLiteralExpr;
      rtn[CXCursor_CXXNullPtrLiteralExpr] = ccs::CursorKind::CXXNullPtrLiteralExpr;
      rtn[CXCursor_CXXThisExpr] = ccs::CursorKind::CXXThisExpr;
      rtn[CXCursor_CXXThrowExpr] = ccs::CursorKind::CXXThrowExpr;
      rtn[CXCursor_CXXNewExpr] = ccs::CursorKind::CXXNewExpr;
      rtn[CXCursor_CXXDeleteExpr] = ccs::CursorKind::CXXDeleteExpr;
      rtn[CXCursor_UnaryExpr] = ccs::CursorKind::UnaryExpr;
      rtn[CXCursor_ObjCStringLiteral] = ccs::CursorKind::ObjCStringLiteral;
      rtn[CXCursor_ObjCEncodeExpr] = ccs::CursorKind::ObjCEncodeExpr;
      rtn[CXCursor_ObjCSelectorExpr] = ccs::CursorKind::ObjCSelectorExpr;
      rtn[CXCursor_ObjCProtocolExpr] = ccs::CursorKind::ObjCProtocolExpr;
      rtn[CXCursor_ObjCBridgedCastExpr] = ccs::CursorKind::ObjCBridgedCastExpr;
      rtn[CXCursor_ObjCBoolLiteralExpr] = ccs::CursorKind::ObjCBoolLiteralExpr;
      
      // statements
      rtn[CXCursor_UnexposedStmt] = ccs::CursorKind::UnexposedStmt;
      rtn[CXCursor_LabelStmt] = ccs::CursorKind::LabelStmt;
      rtn[CXCursor_CaseStmt] = ccs::CursorKind::CaseStmt;
      rtn[CXCursor_DefaultStmt] = ccs::CursorKind::DefaultStmt;
      rtn[CXCursor_IfStmt] = ccs::CursorKind::IfStmt;
      rtn[CXCursor_SwitchStmt] = ccs::CursorKind::SwitchStmt;
      rtn[CXCursor_WhileStmt] = ccs::CursorKind::WhileStmt;
      rtn[CXCursor_DoStmt] = ccs::CursorKind::DoStmt;
      rtn[CXCursor_ForStmt] = ccs::CursorKind::ForStmt;
      rtn[CXCursor_GotoStmt] = ccs::CursorKind::GotoStmt;
      rtn[CXCursor_IndirectGotoStmt] = ccs::CursorKind::IndirectGotoStmt;
      rtn[CXCursor_ContinueStmt] = ccs::CursorKind::ContinueStmt;
      rtn[CXCursor_BreakStmt] = ccs::CursorKind::BreakStmt;
      rtn[CXCursor_ReturnStmt] = ccs::CursorKind::ReturnStmt;
      rtn[CXCursor_AsmStmt] = ccs::CursorKind::AsmStmt;
      rtn[CXCursor_ObjCAtTryStmt] = ccs::CursorKind::ObjCAtTryStmt;
      rtn[CXCursor_ObjCAtCatchStmt] = ccs::CursorKind::ObjCAtCatchStmt;
      rtn[CXCursor_ObjCAtFinallyStmt] = ccs::CursorKind::ObjCAtFinallyStmt;
      rtn[CXCursor_ObjCAtThrowStmt] = ccs::CursorKind::ObjCAtThrowStmt;
      rtn[CXCursor_ObjCAtSynchronizedStmt] = ccs::CursorKind::ObjCAtSynchronizedStmt;
      rtn[CXCursor_ObjCAutoreleasePoolStmt] = ccs::CursorKind::ObjCAutoreleasePoolStmt;
      rtn[CXCursor_ObjCForCollectionStmt] = ccs::CursorKind::ObjCForCollectionStmt;
      rtn[CXCursor_CXXCatchStmt] = ccs::CursorKind::CXXCatchStmt;
      rtn[CXCursor_CXXTryStmt] = ccs::CursorKind::CXXTryStmt;
      rtn[CXCursor_CXXForRangeStmt] = ccs::CursorKind::CXXForRangeStmt;
      rtn[CXCursor_SEHTryStmt] = ccs::CursorKind::SEHTryStmt;
      rtn[CXCursor_SEHExceptStmt] = ccs::CursorKind::SEHExceptStmt;
      rtn[CXCursor_SEHFinallyStmt] = ccs::CursorKind::SEHFinallyStmt;
      rtn[CXCursor_MSAsmStmt] = ccs::CursorKind::MSAsmStmt;
      rtn[CXCursor_NullStmt] = ccs::CursorKind::NullStmt;
      rtn[CXCursor_DeclStmt] = ccs::CursorKind::DeclStmt;
      
      // attributes
      rtn[CXCursor_UnexposedAttr] = ccs::CursorKind::UnexposedAttr;
      rtn[CXCursor_IBActionAttr] = ccs::CursorKind::IBActionAttr;
      rtn[CXCursor_IBOutletAttr] = ccs::CursorKind::IBOutletAttr;
      rtn[CXCursor_IBOutletCollectionAttr] = ccs::CursorKind::IBOutletCollectionAttr;
      rtn[CXCursor_CXXFinalAttr] = ccs::CursorKind::CXXFinalAttr;
      rtn[CXCursor_CXXOverrideAttr] = ccs::CursorKind::CXXOverrideAttr;
      rtn[CXCursor_AnnotateAttr] = ccs::CursorKind::AnnotateAttr;
      rtn[CXCursor_AsmLabelAttr] = ccs::CursorKind::AsmLabelAttr;
      
      // preprocessing
      rtn[CXCursor_PreprocessingDirective] = ccs::CursorKind::PreprocessingDirective;
      rtn[CXCursor_MacroDefinition] = ccs::CursorKind::MacroDefinition;
      rtn[CXCursor_MacroExpansion] = ccs::CursorKind::MacroExpansion;
      rtn[CXCursor_MacroInstantiation] = ccs::CursorKind::MacroInstantiation;
      rtn[CXCursor_InclusionDirective] = ccs::CursorKind::InclusionDirective;
      
      // other
      rtn[CXCursor_TranslationUnit] = ccs::CursorKind::TranslationUnit;
      
      return rtn;
   }
   const CursorKindMap cursorKindMap = initCursorKindMap();
   
   typedef std::map<CXCompletionChunkKind, ccs::CodeCompletionChunkKind::type> ChunkKindMap;
   
   ChunkKindMap initChunkKindMap()
   {
      ChunkKindMap rtn;
      
      rtn[CXCompletionChunk_TypedText] = ccs::CodeCompletionChunkKind::TypedText;
      rtn[CXCompletionChunk_Text] = ccs::CodeCompletionChunkKind::Text;
      rtn[CXCompletionChunk_Placeholder] = ccs::CodeCompletionChunkKind::Placeholder;
      rtn[CXCompletionChunk_Informative] = ccs::CodeCompletionChunkKind::Informative;
      rtn[CXCompletionChunk_CurrentParameter] = ccs::CodeCompletionChunkKind::CurrentParameter;
      rtn[CXCompletionChunk_ResultType] = ccs::CodeCompletionChunkKind::ResultType;
      rtn[CXCompletionChunk_LeftParen] = ccs::CodeCompletionChunkKind::LeftParen;
      rtn[CXCompletionChunk_RightParen] = ccs::CodeCompletionChunkKind::RightParen;
      rtn[CXCompletionChunk_LeftBracket] = ccs::CodeCompletionChunkKind::LeftBracket;
      rtn[CXCompletionChunk_RightBracket] = ccs::CodeCompletionChunkKind::RightBracket;
      rtn[CXCompletionChunk_LeftBrace] = ccs::CodeCompletionChunkKind::LeftBrace;
      rtn[CXCompletionChunk_RightBrace] = ccs::CodeCompletionChunkKind::RightBrace;
      rtn[CXCompletionChunk_LeftAngle] = ccs::CodeCompletionChunkKind::LeftAngle;
      rtn[CXCompletionChunk_RightAngle] = ccs::CodeCompletionChunkKind::RightAngle;
      rtn[CXCompletionChunk_Comma] = ccs::CodeCompletionChunkKind::Comma;
      rtn[CXCompletionChunk_Colon] = ccs::CodeCompletionChunkKind::Colon;
      rtn[CXCompletionChunk_SemiColon] = ccs::CodeCompletionChunkKind::SemiColon;
      rtn[CXCompletionChunk_Equal] = ccs::CodeCompletionChunkKind::Equal;
      rtn[CXCompletionChunk_HorizontalSpace] = ccs::CodeCompletionChunkKind::Space;
      rtn[CXCompletionChunk_VerticalSpace] = ccs::CodeCompletionChunkKind::Newline;
      
      return rtn;
   }
   const ChunkKindMap chunkKindMap = initChunkKindMap();
   
   bool chunkHasText(ccs::CodeCompletionChunkKind::type ccsKind)
   {
      switch (ccsKind)
      {
         case ccs::CodeCompletionChunkKind::TypedText:
         case ccs::CodeCompletionChunkKind::Text:
         case ccs::CodeCompletionChunkKind::Placeholder:
         case ccs::CodeCompletionChunkKind::Informative:
         case ccs::CodeCompletionChunkKind::CurrentParameter:
         case ccs::CodeCompletionChunkKind::ResultType:
         {
            return true;
         }
         
         case ccs::CodeCompletionChunkKind::LeftParen:
         case ccs::CodeCompletionChunkKind::RightParen:
         case ccs::CodeCompletionChunkKind::LeftBracket:
         case ccs::CodeCompletionChunkKind::RightBracket:
         case ccs::CodeCompletionChunkKind::LeftBrace:
         case ccs::CodeCompletionChunkKind::RightBrace:
         case ccs::CodeCompletionChunkKind::LeftAngle:
         case ccs::CodeCompletionChunkKind::RightAngle:
         case ccs::CodeCompletionChunkKind::Comma:
         case ccs::CodeCompletionChunkKind::Colon:
         case ccs::CodeCompletionChunkKind::SemiColon:
         case ccs::CodeCompletionChunkKind::Equal:
         case ccs::CodeCompletionChunkKind::Space:
         case ccs::CodeCompletionChunkKind::Newline:
         {
            return false;
         }
      }
   }
}

CodeCompletionService::CodeCompletionService(
   TranslationUnitManager& tuManager)
   : tuManager_(tuManager)
{
}

CodeCompletionService::~CodeCompletionService()
{
}

ccs::CodeCompletionResponse CodeCompletionService::process(
   const ccs::CodeCompletionRequest& request)
{
   ClangTranslationUnit* tu = tuManager_.translationUnit(request.filename.c_str());
   if (tu == NULL || !tu->isValid())
   {
      qDebug("unable to get TU for [%s]", request.filename.c_str());
      return ccs::CodeCompletionResponse();
   }
   
   QTime timer;
   timer.start();
   
   qDebug("getting completion for '%s' at %d:%d", request.filename.c_str(),
          request.line, request.column);
   
   CXCodeCompleteResults* results =
      clang_codeCompleteAt(*tu,
                           request.filename.c_str(),
                           request.line, request.column,
                           /* unsaved files */ NULL, 0,
                           clang_defaultCodeCompleteOptions());
   if (results == NULL)
   {
      qWarning("Code completion failed");
      return ccs::CodeCompletionResponse();
   }
   
   qDebug("found %d completion results in %dms",
          results->NumResults, timer.elapsed());
   
   ccs::CodeCompletionResponse response;
   response.__set_request(request);
   
   for (unsigned int i = 0; i < results->NumResults; ++i)
   {
      const CXCompletionResult& result = results->Results[i];
      
      ccs::CodeCompletionCandidate candidate;
      
      unsigned int numChunks = clang_getNumCompletionChunks(result.CompletionString);
      if (numChunks == 0)
      {
         // no point in doing anything if we don't have at least TypedText
         continue;
      }
      
      for (unsigned int j = 0; j < numChunks; ++j)
      {
         CXCompletionChunkKind clangKind = clang_getCompletionChunkKind(result.CompletionString, j);
      
         ChunkKindMap::const_iterator chunkKindIt = chunkKindMap.find(clangKind);
         if(chunkKindIt != chunkKindMap.end())
         {
            ccs::CodeCompletionChunk ccsChunk;
            ccsChunk.__set_kind(chunkKindIt->second);
            
            if (chunkHasText(ccsChunk.kind))
            {
               ClangString chunkText(clang_getCompletionChunkText(result.CompletionString, j));
               ccsChunk.__set_text(chunkText.toStdString());
            }
            
            candidate.chunks.push_back(ccsChunk);
         }
      }
      
      CXCursorKind parentKind;
      ClangString parentContext(clang_getCompletionParent(result.CompletionString, &parentKind));
      candidate.__set_parentContext(parentContext.toStdString());
      
      CursorKindMap::const_iterator parentKindIt = cursorKindMap.find(parentKind);
      if (parentKindIt != cursorKindMap.end())
      {
         candidate.__set_parentKind(parentKindIt->second);
      }
      
      response.results.push_back(candidate);
   }
   
   clang_disposeCodeCompleteResults(results);
   
   if (false)
   {
      boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> memBuffer(
         new apache::thrift::transport::TMemoryBuffer());
      boost::shared_ptr<apache::thrift::protocol::TProtocol> debugProto(
         new apache::thrift::protocol::TDebugProtocol(memBuffer));
         // new apache::thrift::protocol::TJSONProtocol(memBuffer));
      
      response.write(debugProto.get());
      qDebug("response:\n-- START --\n%s\n-- END --", memBuffer->getBufferAsString().c_str());
   }
   
   return response;
}
