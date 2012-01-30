
namespace cpp ccs
namespace py ccs

struct CodeCompletionRequest {
   1: required string filename;
   2: required i32 line;
   3: required i32 column;
}

struct CodeCompletionResponse {
   1: optional CodeCompletionRequest request;
   2: required list<string> results;
}

service CCS {

   void ping();
   
   CodeCompletionResponse codeCompletion(1:CodeCompletionRequest request);

}
