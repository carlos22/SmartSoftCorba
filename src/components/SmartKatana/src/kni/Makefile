TARGETS			=	src/common src/Base src/InvKin src/LM src/kni_wrapper
TARGETS_B		=	$(foreach target,$(TARGETS),$(target).build)
TARGETS_C		=	$(foreach target,$(TARGETS),$(target).clean)


all:
	$(MAKE) $(TARGETS_B)

clean:
	$(MAKE) $(TARGETS_C)

$(TARGETS_B):
	$(MAKE) -C $(@:.build=)

$(TARGETS_C):
	$(MAKE)	-C $(@:.clean=) clean
