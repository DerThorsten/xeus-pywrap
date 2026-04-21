from .errors import ShakespeareRuntimeError

from xpywrap import get_interpreter

class BasicOutputManager:
    def output_number(self, number):
        get_interpreter().publish_stdout(_code_to_character(number))

    def output_character(self, character_code):
        get_interpreter().publish_stdout(_code_to_character(character_code))
        # print(_code_to_character(character_code), end="")


class VerboseOutputManager:
    def output_number(self, number):
        #print(f"Outputting number: {str(number)}")
        get_interpreter().publish_stdout(f"Outputting number: {str(number)}")

    def output_character(self, character_code):
        char = _code_to_character(character_code)
        #print(f"Outputting character: {repr(char)}")
        get_interpreter().publish_stdout(f"Outputting character: {repr(char)}")


def _code_to_character(character_code):
    try:
        return chr(character_code)
    except ValueError:
        raise ShakespeareRuntimeError("Invalid character code: " + str(character_code))
