import xpywrap


import io
from contextlib import redirect_stdout


class Interpreter(xpywrap.BlockingInterpreterBase):

    def __init__(self):
        super().__init__()

      

    def kernel_info_request(self):
        return xpywrap.create_kernel_info_reply(
            implementation="xeus-echo",
            implementation_version="0.1.0",
            language_name="echo",
            language_mimetype="plain/text",
            language_file_extension=".xe",
        )
    def execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        self.publish_stdout(code )
        return xpywrap.create_successful_reply()


def make_interpreter(*args, **kwargs):
    return Interpreter()