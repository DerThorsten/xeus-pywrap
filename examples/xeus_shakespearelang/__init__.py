import xpywrap

from shakespearelang.shakespeare import Shakespeare

import io
from contextlib import redirect_stdout


class Interpreter(xpywrap.InterpreterBase):

    def __init__(self):
        super().__init__()

        # redirect print  to custom function

        def print_to_stdout(*args):
            txt = " ".join([str(arg) for arg in args])
            self.publish_stream('stdout', txt)
        
            print("redirecting print to stdout")
            sys.stdout = print_to_stdout


    def kernel_info_request(self):
        kernel_res = {}
        kernel_res["status"] = "ok"
        kernel_res["protocol_version"] = "5.3"
        kernel_res["implementation"] = "shakespearelang"
        kernel_res["implementation_version"] = "0.1.0"
        kernel_res["language_info"] = {}
        kernel_res["language_info"]["name"] = "javascript"
        kernel_res["language_info"]["version"] = "ES6"
        kernel_res["language_info"]["mimetype"] = "application/javascript"
        kernel_res["language_info"]["file_extension"] = ".js"
        kernel_res["language_info"]["pygments_lexer"] = ""
        kernel_res["language_info"]["codemirror_mode"] = ""
        kernel_res["language_info"]["nbconvert_exporter"] = ""
        kernel_res["banner"] = "shakespearelang"
        kernel_res["debugger"] = False
        kernel_res["help_links"] = ["a"]
        return kernel_res
    
    def execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        # print("execute_request", execution_count, code, silent, store_history, user_expressions, allow_stdin)

        with io.StringIO() as buf, redirect_stdout(buf):

            shakespeare = Shakespeare(code)
            res = shakespeare.run()
            output = buf.getvalue()
            self.publish_stream('stdout', output)


        return {"status": "ok", "execution_count": execution_count, "payload": [], "user_expressions": {}}



def make_interpreter(*args, **kwargs):
    print("make_interpreter", args, kwargs)
    return Interpreter()