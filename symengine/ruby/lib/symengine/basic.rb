require 'set'

module SymEngine
  class Basic
    def free_symbols
      self.pr_free_symbols.to_set
    end
  end
end
