from .errors import ShakespeareRuntimeError
import sys

from xpywrap import blocking_input_request

class BasicInputManager:
    def __init__(self):
        self._input_buffer = ""

    def consume_numeric_input(self):
        try:
            self._ensure_input_buffer(prompt="Taking input number: ")
        except EOFError:
            raise ShakespeareRuntimeError("End of file encountered.") from None

        number = self._consume_digits()
        self._consume_newline_if_present()

        return number

    def _consume_newline_if_present(self):
        if self._input_buffer and self._input_buffer[0] == "\n":
            self._input_buffer = self._input_buffer[1:]

    def _consume_digits(self):
        number_input = ""
        while self._input_buffer and self._input_buffer[0].isdigit():
            number_input += self._input_buffer[0]
            self._input_buffer = self._input_buffer[1:]

        if len(number_input) == 0:
            raise ShakespeareRuntimeError("No numeric input was given.")

        return int(number_input)

    def consume_character_input(self):
        try:
            self._ensure_input_buffer(prompt="Taking input character: ")
        except EOFError:
            return -1

        value = ord(self._input_buffer[0])
        self._input_buffer = self._input_buffer[1:]
        return value

    def _ensure_input_buffer(self, prompt):
        if not self._input_buffer:
            # We want all output that has already happened to appear before we
            # ask the user for input
            sys.stdout.flush()
            self._input_buffer = blocking_input_request(prompt)
            if not self._input_buffer:
                raise EOFError()


class InteractiveInputManager:
    def consume_numeric_input(self):
        try:
            value = int(blocking_input_request("Taking input number: "))
        except ValueError:
            raise ShakespeareRuntimeError("No numeric input was given.")

        return value

    def consume_character_input(self):
        value = blocking_input_request("Taking input character: ")
        if value == "EOF":
            return -1
        elif value == "":
            return ord("\n")
        else:
            return ord(value[0])
