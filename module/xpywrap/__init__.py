from .interpreter_base import InterpreterBase # noqa
from .response import * # noqa
import _xpywrap #noqa

import asyncio

import nest_asyncio



async def  async_pseudo_event_loop(server):
    try:
        while not server.stopped():
            # print("polling")
            server.poll(0)
            await asyncio.sleep(0)

    except Exception as e:
        print("exception", e)


def runner(server):
    try:
        # #asyncio.run(async_pseudo_event_loop(server))
        # # run as task
        # asyncio.create_task(async_pseudo_event_loop(server))

        # check if we are in an event loop
        if not asyncio.get_event_loop().is_running():
            print("not in event loop")
            # run the event loop
            asyncio.run(async_pseudo_event_loop(server))
        else:
            print("in event loop, nest_asyncio.apply()")
            nest_asyncio.apply()

            # run as task
            task = asyncio.create_task(async_pseudo_event_loop(server))

            loop = asyncio.get_event_loop()
            loop.run_until_complete(task)



    except Exception as e:
        print("exception in runner",e)