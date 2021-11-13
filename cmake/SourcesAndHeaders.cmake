set(sources
    src/parser.c
    src/tokenizer.c
)

set(exe_sources
		src/main.c
		${sources}
)

set(headers
    include/parser.h
    include/tokenizer.h
)
