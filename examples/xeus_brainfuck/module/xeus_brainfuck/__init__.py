import xpywrap
import sys
import io
from contextlib import redirect_stdout
from .brainfuck.brainfuck import evaluate

class Interpreter(xpywrap.BlockingInterpreterBase):

    def __init__(self):
        super().__init__()

    
    def configure(self):
        # monkey patch  sys.stdout.write st. it uses the publish_stream method of the interpreter implementation to send output to the frontend.
        
        def write(*args, **kwargs):
            text = ''.join(str(arg) for arg in args)
            self.publish_stdout(text)
        
        sys.stdout.write = write

      

    def kernel_info_request(self):
        return xpywrap.create_kernel_info_reply(
            implementation="xeus-brainfuck",
            implementation_version="0.1.0",
            language_name="brainfuck",
            language_mimetype="text/plain",
            language_file_extension=".bf",
        )
    def execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        evaluate(code)
        return xpywrap.create_successful_reply()


def make_interpreter(*args, **kwargs):
    return Interpreter()