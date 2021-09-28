import os, sys
from glob import glob

mapping_dict = {
    "batchNormalization": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-batchnorm",
    "clamp": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-clamp",
    "concat": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-concat",
    "conv2d": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-conv2d",
    "add": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "sub": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "mul": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "div": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "max": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "min": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "pow": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-binary",
    "abs": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "ceil": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "cos": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "floor": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "log": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "neg": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "sin": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "tan": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary",
    "elu": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-elu",
    "gemm": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-gemm",
    "gru": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-gru",
    "gruCell": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-grucell",
    "hardSigmoid": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-hard-sigmoid",
    "hard_swish": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-hard-swish",
    "instanceNormalization": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-instancenorm",
    "leakyRelu": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-leakyrelu",
    "matmul": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-matmul",
    "linear": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-linear",
    "pad": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-pad",
    "pooling operations": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-pool2d",
    "reduction operations": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-reduce",
    "relu": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-relu",
    "resample": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-resample",
    "reshape": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-reshape",
    "sigmoid": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-sigmoid",
    "slice": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-slice",
    "softmax": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-softmax",
    "softplus": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-softplus",
    "softsign": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-softsign",
    "split": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-split",
    "squeeze": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-squeeze",
    "tanh": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-tanh",
    "transpose": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-transpose",
    "cts": "https://webmachinelearning.github.io/webnn/#api-neuralnetworkcontext-binary",
    "element-wise unary operations": "https://webmachinelearning.github.io/webnn/#api-mlgraphbuilder-unary"
}

def wpt_converter(input_path, output_path):
    all_js_files_list = list()
    if os.path.isdir(input_path):
        all_js_files_list = glob(input_path + "/*.js")
    elif os.path.isfile(input_path):
        all_js_files_list.append(input_path)
    else:
        print("input path error")
        return
    for js_name in all_js_files_list:
        target = os.path.join(output_path, js_name.split(os.path.sep)[-1].split(".")[0] + ".html")
        title_text = js_name.split(os.path.sep)[-1].split(".")[0]
        if title_text == "batch_norm":
            title_text = "batchNormalization"
            link_text = mapping_dict[title_text]
            title_text = "batchNormalization operation"
        elif title_text == "instance_norm":
            title_text = "instanceNormalization"
            link_text = mapping_dict[title_text]
            title_text = "instanceNormalization operation"
        elif title_text == "leaky_relu":
            title_text = "leakyRelu"
            link_text = mapping_dict[title_text]
            title_text = "leakyRelu operation"
        elif title_text == "pool2d":
            title_text = "pooling operations"
            link_text = mapping_dict[title_text]
        elif title_text == "reduce":
            title_text = "reduction operations"
            link_text = mapping_dict[title_text]
        elif title_text =="unary":
            title_text = "element-wise unary operations"
            link_text = mapping_dict[title_text]
        else:
            link_text = mapping_dict[title_text]
            title_text = title_text + " operation"
        html_head_content = """<!doctype html>
<meta charset=utf-8>
<title>test %s</title>
<link rel="help" href="%s">
<script src="../resources/testharness.js"></script>
<script src="../resources/testharnessreport.js"></script>
<script src="./dist/webnn-polyfill.js"></script>
<div id=log></div>
<script type="module">
  'use strict';
  import * as utils from './utils.js';
  let builder;
  setup(() => {
    const context = navigator.ml.createContext();
    builder = new MLGraphBuilder(context);
  })""" % (title_text, link_text)

        with open(target, 'w') as html_writer:
            html_writer.writelines(html_head_content)
            with open(js_name, 'r') as js_reader:
                js_content_list = js_reader.readlines()
                global case_name
                for js_line in js_content_list:
                    if js_line.startswith("'use strict';"):
                        continue
                    elif "import" in js_line:
                        continue
                    elif js_line.startswith("/* eslint-disable max-len */"):
                        continue
                    elif js_line.startswith("describe") and "function()" in js_line:
                        continue
                    elif js_line.startswith("  const context = navigator.ml.createContext();"):
                        continue
                    elif js_line.startswith("  it("):
                        if "async function() {" in js_line:
                            case_name = js_line.split("it(")[1].split(", async function()")[0]
                        elif "function() {" in js_line:
                            case_name = js_line.split("it(")[1].split(", function()")[0]
                        else:
                            print("Unknown case name!")
                            return
                        js_line = js_line.replace(js_line, "  test(() => {\r\n")
                    elif js_line.startswith("    const builder = new MLGraphBuilder(context);"):
                        continue
                    elif js_line.startswith("      function() {"):
                        continue
                    elif js_line.startswith("  });\n"):
                        js_line = js_line.replace("  });\n", "  }, " + case_name + ");\n")
                    elif js_line.startswith("});\n"):
                        continue
                    elif "unary.js" in js_name:
                        if js_line.startswith("  function test(op, input, expected, shape) {"):
                            js_line = js_line.replace("  function test(op, input, expected, shape) {", "  function testUnary(op, input, expected, shape) {")
                        elif js_line.startswith("    test("):
                            js_line = js_line.replace("    test(", "    testUnary(")
                    html_writer.writelines(js_line)
            html_writer.writelines('</script>\n')


if __name__ == "__main__":
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    wpt_converter(input_path, output_path)
