// Fuzzer for //graphviz heavily inspired by graphviz-server.
// See GvRenderer::RenderInternal and GvRenderer::GvRenderFormat in
// //visualization/graphviz_server/internal/gv-renderer.cc

#include <fuzzer/FuzzedDataProvider.h>

#include <string>

#include "lib/gvc/gvc.h"


// Currently hits leaks and use-after-free if freed and recreated.
GVC_t* gv_context = gvContextPlugins(lt_preloaded_symbols, 0);

const char* FORMATS[] = {
  "bmp",       "canon",     "dot",  "gv",   "xdot",  "xdot1.2", "xdot1.4",
  "cgimage",   "cmap",      "eps",  "exr",  "fig",   "gd",      "gd2",
  "gif",       "gtk",       "ico",  "imap", "cmapx", "imap_np", "cmapx_np",
  "ismap",     "jp2",       "jpg",  "jpeg", "jpe",   "json",    "json0",
  "dot_json",  "xdot_json", "pct",  "pict", "pdf",   "pic",     "plain",
  "plain-ext", "png",       "pov",  "ps",   "ps2",   "psd",     "sgi",
  "svg",       "svgz",      "tga",  "tif",  "tiff",  "tk",      "vml",
  "vmlz",      "vrml",      "wbmp", "webp", "xlib",  "x11"};
const char* ENGINES [] = {"circo", "dot",       "fdp",  "neato",
                          "osage", "patchwork", "sfdp", "twopi"};


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < 3) return 0;

  FuzzedDataProvider provider(data, size);
  const char* format = provider.PickValueInArray(FORMATS);
  const char* engine = provider.PickValueInArray(ENGINES);
  agseterr(AGMAX);  // Don't print to stderr.

  auto graph = agmemread(provider.ConsumeRemainingBytesAsString().c_str());

  if (!graph){
    return 0;
  }

  if (int result = gvLayout(gv_context, graph, engine)) {
    size_t output_length = 0;
    char* output_buffer = NULL;
    // open_memstream() will malloc() output_buffer for us.
    FILE* f = open_memstream(&output_buffer, &output_length);
    gvRender(gv_context, graph, format, f);
    fclose(f);
    free(output_buffer);
  }

  gvFreeLayout(gv_context, graph);
  agclose(graph);
  return 0;
}
