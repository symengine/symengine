require 'set'

module SymEngine
  class Basic
    def free_symbols
      pr_free_symbols.to_set
    end

    def hash
      [self.to_s].hash
    end
  end
end
