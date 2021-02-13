define chain-commands
@for subdir in $(SUBDIR); do \
	$(MAKE) -C $$subdir $@; \
done
endef

all:
	$(chain-commands)

clean:
	$(chain-commands)

rebuild: clean all
