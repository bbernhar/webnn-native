import os, sys
from glob import glob

def wpt_model_converter(input_path, output_path):
    all_js_files_list = list()
    if os.path.isdir(input_path):
        all_js_files_list = glob(input_path + "/*.js")
    elif os.path.isfile(input_path):
        all_js_files_list.append(input_path)
    else:
        print("Input path error")
        return
    for js_name in all_js_files_list:
        if "squeezenet" in js_name:
            target = os.path.join(output_path, js_name.split(os.path.sep)[-1].split("_")[0] + "_" + js_name.split(os.path.sep)[-1].split("_")[1].split(".")[0] + ".html")
        else:
            target = os.path.join(output_path, js_name.split(os.path.sep)[-1].split(".")[0] + ".html")
        test_data_dir = js_name.split(os.path.sep)[-1].split(".js")[0]
        title_text = test_data_dir.replace("_"," ")
        html_head_content = """<!doctype html>
<meta charset=utf-8>
<title>test %s</title>
<link rel="help" href="https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder">
<script src="../resources/testharness.js"></script>
<script src="../resources/testharnessreport.js"></script>
<script src="./dist/webnn-polyfill.js"></script>
<div id=log></div>
<script type="module">
  'use strict';
  import * as utils from './utils.js';
  let graph;
  let fusedGraph;
  let builder;
  const url = import.meta.url;
  const testDataDir = 'https://webmachinelearning.github.io/test-data/models/%s';\n
  setup(() => {
    const context = navigator.ml.createContext();
    builder = new MLGraphBuilder(context);
  },  { explicit_timeout: true });""" % (title_text, test_data_dir)
        with open(target, 'w') as html_writer:
            html_writer.writelines(html_head_content)
            with open(js_name, 'r') as js_reader:
                js_content_list = js_reader.readlines()
                global case_tag, before_tag, after_tag, case_name, case_condition1, case_condition2, case_condition3, total_ignored_line, after_total_ignored_line, before_line_index, after_line_index, current_line_index
                case_tag = False
                before_tag = False
                after_tag = False
                case_name = ""
                case_condition1 = ""
                case_condition2 = ""
                case_condition3 = ""
                total_ignored_line = 4
                after_total_ignored_line = 15
                before_line_index = None
                after_line_index = None
                if "  before(async () => {\n" in js_content_list:
                    before_line_index = js_content_list.index("  before(async () => {\n")
                if "  after(async () => {\n" in js_content_list:
                    after_line_index = js_content_list.index("  after(async () => {\n")
                elif "  after(() => {\n" in js_content_list:
                    after_line_index = js_content_list.index("  after(() => {\n")
                for js_line in js_content_list:
                    current_line_index = js_content_list.index(js_line)
                    if "import" in js_line:
                        continue
                    elif js_line.startswith("'use strict';"):
                        continue
                    elif js_line.startswith("describe") and "function()" in js_line:
                        continue
                    elif js_line.startswith("  //"):
                        continue
                    elif js_line.startswith("  this.timeout"):
                        continue
                    elif js_line.startswith("const assert = chai.assert;"):
                        continue
                    elif js_line.startswith("const testDataDir = '../../test-data/models/"):
                        continue
                    elif js_line.startswith('/* eslint max-len: ["error", {"code": 120}] */'):
                        continue
                    elif js_line.startswith("  let graph;"):
                        continue
                    elif js_line.startswith("  let fusedGraph;"):
                        continue
                    elif js_line.startswith("  let beforeNumBytes;"):
                        continue
                    elif js_line.startswith("  let beforeNumTensors;"):
                        continue
                    elif js_line.startswith("  before(async () => {\n"):
                        before_tag = True
                        continue
                    elif js_line.startswith("    const context = navigator.ml.createContext();"):
                        before_tag = False
                        continue
                    elif js_line.startswith("    const builder = new MLGraphBuilder(context);"):
                        continue
                    elif before_tag:
                        continue
                    elif js_line.startswith("    graph = await build"):
                        case_condition1 = js_line
                        continue
                    elif js_line.startswith("    fused") and ("true;" in js_line):
                        case_condition2 = js_line
                        continue
                    elif js_line.startswith("    fusedGraph = await build"):
                        case_condition3 = js_line
                        continue
                    elif js_line.startswith("  after(() => {") or js_line.startswith("  after(async () => {"):
                        after_tag = True
                        continue
                    elif (js_line.startswith("  async function test")) and ("(graph, inputFile, expectedFile" in js_line):
                        after_tag = False
                    elif after_tag:
                        continue
                    elif js_line.startswith("  it(") or js_line.startswith("  it.skip("):
                        case_tag = True
                        new_js_line = "  promise_test(async () => {\n"
                        if "(fused ops)" in js_line:
                            case_name = js_line.split(",")[0].split("(")[1] + '(' + js_line.split(",")[0].split("(")[2]
                            html_writer.writelines(new_js_line)
                            html_writer.writelines(case_condition2)
                            html_writer.writelines(case_condition3)
                        else:
                            case_name = js_line.split(",")[0].split("(")[1]
                            html_writer.writelines(new_js_line)
                            html_writer.writelines(case_condition1)
                        continue
                    elif js_line.startswith("  });"):
                        if case_tag:
                            js_line = js_line.replace("  });\n", "    }, \'test %s / %s\');\n" % (title_text, case_name.strip("'")))
                        else:
                            continue
                    elif js_line.startswith("});"):
                        continue
                    if (current_line_index > before_line_index) and (current_line_index < after_line_index):
                        html_writer.writelines(js_line[2:])
                    else:
                        html_writer.writelines(js_line)
            html_writer.writelines('</script>\n')


if __name__ == "__main__":
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    wpt_model_converter(input_path, output_path)
