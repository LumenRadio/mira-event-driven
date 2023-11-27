all:
	$(MAKE) -C application

fmt:
	clang-format -i --style=file -- */*.[ch]

.SILENT:
.PHONY: all fmt
