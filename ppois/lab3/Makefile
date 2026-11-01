# Быстрая сборка без CMake.
#   make           — собрать (параллельно, только изменённое), бинарник build/make/fms
#   make run       — собрать и запустить меню
#   make test      — собрать и запустить самопроверку (--test)
#   make release   — собрать с оптимизацией -O2 в build/make-release/fms
#
# Ручной make clean не нужен — уборка автоматическая:
#   - удалили или переименовали .cpp — его старые .o и .d удаляются при следующей сборке;
#   - удалили заголовок — -MP не даёт make упасть на отсутствующем файле;
#   - поменялись флаги компиляции (здесь или через EXTRA=...) — папка сборки очищается сама
#     и всё собирается заново.
#
# Почему быстро:
#   1) все .cpp компилируются параллельно — по числу ядер;
#   2) -MMD -MP записывают, какие заголовки подключает каждый .cpp: после правки одного .h
#      пересобираются только зависящие от него файлы, а без изменений make ничего не делает;
#   3) предкомпилированный заголовок: <iostream>, <string>, <vector> и другие стандартные заголовки
#      разбираются один раз, а не в каждом из ~60 файлов (сборка с нуля почти вдвое быстрее);
#   4) по умолчанию -O0: компилятор не тратит время на оптимизации;
#   5) если установлен ccache, он подключается сам и берёт уже скомпилированное из кэша.

MODE ?= debug
ifeq ($(MODE),release)
  OPT    := -O2
  OUTDIR := build/make-release
else
  OPT    := -O0
  OUTDIR := build/make
endif

CCACHE   := $(shell command -v ccache 2>/dev/null)
export CCACHE_SLOPPINESS := pch_defines,time_macros,include_file_mtime,include_file_ctime
CXX      := $(CCACHE) c++
EXTRA    ?=
BASEFLAGS := -std=c++17 -Wall -Wextra $(OPT) $(EXTRA)
JOBS     := $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
MAKEFLAGS += -j$(JOBS) --no-print-directory

# Предкомпилированный заголовок со стандартной библиотекой. Файл генерируется здесь же,
# в проекте отдельного заголовка нет. У clang и gcc разные форматы и флаги подключения.
STD_HEADERS := iostream sstream string vector iomanip stdexcept cmath cctype climits utility type_traits
PCH_SRC := $(OUTDIR)/std.h
ifneq ($(shell c++ --version 2>/dev/null | grep -c clang),0)
  PCH     := $(OUTDIR)/std.h.pch
  PCHFLAG := -include-pch $(PCH)
else
  PCH     := $(OUTDIR)/std.h.gch
  PCHFLAG := -include $(PCH_SRC)
endif

CXXFLAGS := $(BASEFLAGS) -Isrc -MMD -MP $(PCHFLAG)

SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:src/%.cpp=$(OUTDIR)/%.o)
DEPS := $(OBJS:.o=.d)
BIN  := $(OUTDIR)/fms

# Автоуборка 1: объектные файлы, у которых больше нет исходника (.cpp удалили или переименовали).
STALE := $(filter-out $(OBJS),$(shell find $(OUTDIR) -name '*.o' 2>/dev/null))
ifneq ($(STALE),)
  $(shell rm -f $(STALE) $(STALE:.o=.d))
  $(info Удалены устаревшие объектные файлы: $(STALE))
endif

# Автоуборка 2: в папке сборки хранится строка флагов, с которыми она собрана. Если флаги изменились,
# папка очищается целиком — это и есть автоматический clean, и всё соберётся заново с новыми флагами.
# Сравнивается сам текст, а не время файлов: make на macOS видит время с точностью до секунды
# и при быстрых повторных сборках мог бы счесть часть старых .o свежими.
FLAGS_STAMP := $(OUTDIR)/.flags
FLAGS_NOW   := $(strip $(CXX) $(CXXFLAGS))
ifneq ($(shell cat $(FLAGS_STAMP) 2>/dev/null),$(FLAGS_NOW))
  ifneq ($(wildcard $(FLAGS_STAMP)),)
    $(info Флаги сборки изменились — $(OUTDIR) очищена, всё соберётся заново)
  endif
  $(shell rm -rf $(OUTDIR) && mkdir -p $(OUTDIR) && echo '$(FLAGS_NOW)' > $(FLAGS_STAMP))
endif

.PHONY: all run test release

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Каждый .o зависит от предкомпилированного заголовка: он собирается первым.
$(OUTDIR)/%.o: src/%.cpp $(PCH)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PCH_SRC): Makefile
	@mkdir -p $(dir $@)
	@printf '%s\n' $(foreach h,$(STD_HEADERS),'#include <$(h)>') > $@

$(PCH): $(PCH_SRC)
	$(CXX) $(BASEFLAGS) -x c++-header $< -o $@

run: $(BIN)
	./$(BIN)

test: $(BIN)
	./$(BIN) --test

release:
	$(MAKE) MODE=release

-include $(DEPS)
