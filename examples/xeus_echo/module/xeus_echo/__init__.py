import xpywrap


import io
from contextlib import redirect_stdout


class Interpreter(xpywrap.InterpreterBase):

    def __init__(self):
        super().__init__()

      

    def kernel_info_request(self):
        kernel_res = {}
        kernel_res["status"] = "ok"
        kernel_res["protocol_version"] = "5.3"
        kernel_res["implementation"] = "xeus-echo"
        kernel_res["implementation_version"] = "0.1.0"
        kernel_res["language_info"] = {}
        kernel_res["language_info"]["name"] = "echo"
        kernel_res["language_info"]["version"] = "ES6"
        kernel_res["language_info"]["mimetype"] = "plain/text"
        kernel_res["language_info"]["file_extension"] = ".xe"
        kernel_res["language_info"]["pygments_lexer"] = ""
        kernel_res["language_info"]["codemirror_mode"] = ""
        kernel_res["language_info"]["nbconvert_exporter"] = ""
        kernel_res["banner"] = "xeus-echo"
        kernel_res["debugger"] = False
        kernel_res["help_links"] = [{}]
        return kernel_res
    
    def execute_request(self, cb, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        self.publish_stdout(code )
        cb({"status": "ok", "execution_count": execution_count, "payload": [], "user_expressions": {}})



def make_interpreter(*args, **kwargs):
    return Interpreter()