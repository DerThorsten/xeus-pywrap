import xpywrap
import sys
import io
import builtins
from contextlib import redirect_stdout
from .PyBasic.lexer import Lexer
from .PyBasic.program import Program

class Interpreter(xpywrap.BlockingInterpreterBase):

    def __init__(self):
        super().__init__()

    
    def configure(self):
        
        # monkey patch  sys.stdout.write st. it uses the publish_stream method of the interpreter implementation to send output to the frontend.
        def write(*args, **kwargs):
            text = ''.join(str(arg) for arg in args)
            self.publish_stdout(text)
        sys.stdout.write = write

        # monkey patch global input function to use the frontend's input dialog instead of the terminal
        def custom_input(prompt=""):
            return xpywrap.blocking_input_request(prompt)
        builtins.input = custom_input
      

    def kernel_info_request(self):
        return xpywrap.create_kernel_info_reply(
            implementation="xeus-basic",
            implementation_version="0.1.0",
            language_name="basic",
            # even though its basic, we use the fortran mimetype / syntax highlighting, 
            # since BASIC seems to be non supported by codemirror
            language_mimetype="text/x-fortran",
            pygments_lexer="fortran",
            language_codemirror_mode={'name': 'fortran', 'version': 77},
            language_file_extension=".bas",
        )
    def execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        
        # split in lines
        lexer = Lexer()
        program = Program()
        code_lines = code.splitlines()
        for line in code_lines:
            line = line.replace("\r", "").replace("\n", "").strip()
            tokenlist = lexer.tokenize(line)
            program.add_stmt(tokenlist)

      
        program.execute()


def make_interpreter(*args, **kwargs):
    return Interpreter()