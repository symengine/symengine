require 'spec_helper'

describe 'Arithmetic test cases' do
  before :each do
  end

  it 'test_arit1' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    e = x + y
    e = x * y
    e = SymEngine::Integer.new(2) * x
    e = 2 * x
    e = x + 1
    e = 1 + x
  end

  it 'test_arit2' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert x + x == SymEngine::Integer.new(2) * x
    assert x + x != SymEngine::Integer.new(3) * x
    assert x + y == y + x
    assert x + x == 2 * x
    assert x + x == x * 2
    assert x + x + x == 3 * x
    assert x + y + x + x == 3 * x + y

    assert(!(x + x == 3 * x))
    assert(!(x + x != 2 * x))
  end

  it 'test_arit3' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    expect do
      ('x' * x)
    end.to raise_error(TypeError)
  end

  it 'test_arit4' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert x * x == x**2
    assert x * y == y * x
    assert x * x * x == x**3
    assert x * y * x * x == x**3 * y
  end

  it 'test_arit5' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    e = (x + y)**2
    f = e.expand
    assert e == (x + y)**2
    assert e != x**2 + 2 * x * y + y**2
    expect(e).to be_an_instance_of SymEngine::Basic
    assert f == x**2 + 2 * x * y + y**2
    expect(f).to be_an_instance_of SymEngine::Basic
  end

  it 'test_arit6' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    e = x + y
    assert(e.to_s == 'x + y') || 'y + x'
    e = x * y
    assert(e.to_s == 'x*y') || 'y*x'
    e = Integer(2) * x
    assert e.to_s == '2*x'
    e = 2 * x
    assert e.to_s == '2*x'
  end

  it 'test_arit7' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert x - x == 0
    assert x - y != y - x
    assert 2 * x - x == x
    assert 3 * x - x == 2 * x

    assert 2 * x * y - x * y == x * y
  end

  it 'test_arit8' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    z = SymEngine::Symbol.new('z')
    assert x**y * x**x == x**(x + y)
    assert x**y * x**x * x**z == x**(x + y + z)
    assert x**y - x**y == 0

    assert x**2 / x == x
    assert y * x**2 / (x * y) == x
    assert (2 * x**3 * y**2 * z)**3 / 8 == x**9 * y**6 * z**3
    assert (2 * y**(-2 * x**2)) * (3 * y**(2 * x**2)) == 6
  end

  it 'test_expand1' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    z = SymEngine::Symbol.new('z')
    assert ((2 * x + y)**2).expand == 4 * x**2 + 4 * x * y + y**2
    assert (x**2)**3 == x**6
    assert ((2 * x**2 + 3 * y)**2).expand == 4 * x**4 + 12 * x**2 * y + 9 * y**2
    assert ((2 * x / 3 + y / 4)**2).expand == 4 * x**2 / 9 + x * y / 3 + y**2 / 16
  end

  it 'test_arit9' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert 1 / x == 1 / x
    assert 1 / x != 1 / y
  end

  it 'test_expand2' do
    y = SymEngine::Symbol.new('y')
    z = SymEngine::Symbol.new('z')
    assert ((1 / (y * z) - y * z) * y * z).expand == 1 - (y * z)**2
  end

  it 'test_expand3' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert ((1 / (x * y) - x * y + 2) * (1 + x * y)).expand == 3 + 1 / (x * y) + x * y - (x * y)**2
  end

  it 'test_args' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    assert (x**2).args == [x, 2]
    assert (x**2 + 5).args == [5, x**2]
    assert (x**2 + 2 * x * y + 5).args == [5, 2 * x * y, x**2]
    assert (2 * x**2).args == [2, x**2]
    assert (2 * x**2 * y).args == [2, x**2, y]
  end

  it 'test_free_Symbols' do
    x = SymEngine::Symbol.new('x')
    y = SymEngine::Symbol.new('y')
    z = SymEngine::Symbol.new('z')
    assert (x**2).free_symbols == Set.new([x])
    assert (x**y + z).free_symbols == Set.new([x, y, z])
  end
end
