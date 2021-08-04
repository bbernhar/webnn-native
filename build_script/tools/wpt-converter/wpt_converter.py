import os, sys
import shutil
from shutil import copyfile, copyfileobj
from glob import glob


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
        source = "html_head_part.html"
        target = os.path.join(output_path, js_name.split(os.path.sep)[-1].split(".")[0] + ".html")
        try:
            html_generator = copyfile(source, target)
        except:
            print("copy file failed")
        with open(html_generator, "a+") as html_writer:
            with open(js_name, 'r') as js_reader:
                js_content_list = js_reader.readlines()
                global case_name
                for js_line in js_content_list:
                    if "import" in js_line:
                        continue
                    elif js_line.startswith("describe") and "function()" in js_line:
                        continue
                    elif js_line.startswith("  it("):
                        case_name = js_line.split(",")[0].split("(")[1]
                        js_line=js_line.replace(js_line, "  test(() => {\n")
                    elif js_line.startswith("      function() {"):
                        continue
                    elif js_line.startswith("  });\n"):
                        js_line = js_line.replace("  });\n", "  }," + case_name + ");\n")
                    elif js_line.startswith("});\n"):
                        continue
                    elif js_line == '    ':
                        js_line = js_line.strip()
                    js_line=js_line.replace("'use strict';\n", "")
                    js_line=js_line.replace("const context = navigator.ml.createContext();\n", "")
                    js_line=js_line.replace("const builder = new MLGraphBuilder(context);\n", "\n")
                    js_line=js_line.replace("utils.", "")
                    html_writer.writelines(js_line)    
            html_writer.writelines('</script>\n')


if __name__ == "__main__": 
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    wpt_converter(input_path, output_path)