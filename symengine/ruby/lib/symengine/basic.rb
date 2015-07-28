require 'set'

module SymEngine
  class Basic
    def free_symbols
      pr_free_symbols.to_set
    end
  end
end

class SymEngine::Basic
  def coerce(other)
    if other.is_a? Integer
      [SymEngine::Integer.new(other), self]
    elsif other.is_a? Rational
      [SymEngine::Rational.new(other), self]
    end
  end
end
