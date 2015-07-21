class Fixnum
  alias_method :old_add, :+
  def +(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) + other
    else
      old_add other
    end
  end

  alias_method :old_sub, :-
  def -(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) - other
    else
      old_sub other
    end
  end

  alias_method :old_mul, :*
  def *(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) * other
    else
      old_mul other
    end
  end

  alias_method :old_div, :/
  def /(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) / other
    else
      old_div other
    end
  end

  alias_method :old_pow, :**
  def **(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) ** other
    else
      old_pow other
    end
  end
end

class Bignum
  alias_method :old_add, :+
  def +(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) + other
    else
      old_add other
    end
  end

  alias_method :old_sub, :-
  def -(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) - other
    else
      old_sub other
    end
  end

  alias_method :old_mul, :*
  def *(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) * other
    else
      old_mul other
    end
  end

  alias_method :old_div, :/
  def /(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) / other
    else
      old_div other
    end
  end

  alias_method :old_pow, :**
  def **(other)
    if other.is_a? SymEngine::Basic
      SymEngine::Integer.new(self) ** other
    else
      old_pow other
    end
  end
end
