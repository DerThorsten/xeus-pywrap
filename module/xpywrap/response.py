from .constants import ( PROTOCOL_VERSION,)

def  language_info_response(
        name,
        version,
        mimetype,
        file_extension,
        pygments_lexer = "",
        codemirror_mode = "",
        nbconvert_exporter = ""
):
    return {
        "name": name,
        "version": version,
        "mimetype": mimetype,
        "file_extension": file_extension,
        "pygments_lexer": pygments_lexer,
        "codemirror_mode": codemirror_mode,
        "nbconvert_exporter": nbconvert_exporter
    }


def kernel_info_response(
        implementation,
        language_info,
        implementation_version="0.1.0",
        protocol_version=PROTOCOL_VERSION,
        banner=None,
        help_links = None,
        debugger = False
):
    if help_links is None:
        help_links = []
    
    if banner is None:
        banner = f"{implementation} kernel"

    return {
        "status": "ok",
        "protocol_version": protocol_version,
        "implementation": implementation,
        "implementation_version": implementation_version,
        "language_info": language_info,
        "banner": banner,
        "help_links": help_links,
        "debugger": debugger
    }


def shutdown_response(restart=False):
    return {
        "status": "ok",
        "restart": restart
    }


def execute_error_repsonse(
        execution_count,
        ename = "Unknown error",
        evalue = "Unknown error",
        traceback = None
):
    if traceback is None:
        traceback = []
    
    return {
        "status": "error",
        "execution_count": execution_count,
        "ename": ename,
        "evalue": evalue,
        "traceback": traceback
    }

def execute_ok_response(
        execution_count,
        status = "ok",
        payload = None,
        user_expressions = None
):
    if user_expressions is None:
        user_expressions = {}
    if payload is None:
        payload = []
    return {
        "status": status,
        "execution_count": execution_count,
        "payload": payload,
        "user_expressions": user_expressions
    }