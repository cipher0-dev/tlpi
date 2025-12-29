
.PHONY: format

format:
	find . -iname '*.c' ! -path './**/ref/*' ! -path './ref/*' -print0 | xargs -0 clang-format -i
