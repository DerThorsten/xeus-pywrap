import xpywrap

from .shakespearelang.shakespeare import Shakespeare

import io
from contextlib import redirect_stdout


class Interpreter(xpywrap.BlockingInterpreterBase):

    def kernel_info_request(self):
        return xpywrap.create_kernel_info_reply(
            implementation="shakespearelang",
            implementation_version="0.1.0",
            language_name="shakespearelang",
            language_mimetype="text/x-shakespearelang",
            language_file_extension=".spl",
        )
    
    def execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):

        f = io.StringIO(code)
        with io.StringIO() as buf, redirect_stdout(buf):

            shakespeare = Shakespeare(f.read())
            res = shakespeare.run()
            output = buf.getvalue()
            if output:
                self.publish_stream('stdout', output)

        return xpywrap.create_successful_reply()


def make_interpreter(*args, **kwargs):
    return Interpreter()