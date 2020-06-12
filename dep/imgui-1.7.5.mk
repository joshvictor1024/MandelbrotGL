DEP += $(strip $(patsubst $(DEPDIR)/imgui-1.7.5/%.cpp, $(INTDIR)/%.d, $(wildcard $(DEPDIR)/imgui-1.7.5/*.cpp)))

$(INTDIR)/%.d: $(DEPDIR)/imgui-1.7.5/%.cpp
	$(CXX) -MM -MF $@ $(DEPDIR)/imgui-1.7.5/$*.cpp $(INCDIRFLAGS) -MT $(INTDIR)/$*.o
	echo		$(CXX) $(CXXFLAGS) $(DEPDIR)/imgui-1.7.5/$*.cpp -c $(INCDIRFLAGS) -o $$@ >> $@
