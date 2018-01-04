// Fuzzer for //graphviz heavily inspired by graphviz-server.
// See GvRenderer::RenderInternal and GvRenderer::GvRenderFormat in
// //visualization/graphviz_server/internal/gv-renderer.cc

#include <string>

#include "lib/gvc/gvc.h"



// Currently hits leaks and use-after-free if freed and recreated.
GVC_t* gv_context = gvContextPlugins(lt_preloaded_symbols, 0);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 3) return 0;
  std::string s(reinterpret_cast<const char*>(data), size);
  
  // perhaps derive from the fuzz input?)
  std::string engine = "dot";  // e.g. dot, circo, neato, twopi, etc.
  std::string format = "svg";  // e.g. svg, png, ps, dot, plain, etc.
  agseterr(AGMAX);  // Don't print to stderr.
  if (auto graph = agmemread(s.c_str())) {
    int result = gvLayout(gv_context, graph, engine.c_str());

    if (result) {
      size_t output_length = 0;
      char* output_buffer = NULL;
      // open_memstream() will malloc() output_buffer for us.
      FILE* f = open_memstream(&output_buffer, &output_length);
      gvRender(gv_context, graph, format.c_str(), f);
      fclose(f);
      free(output_buffer);
    }
    gvFreeLayout(gv_context, graph);
    agclose(graph);
  }
  return 0;
}
