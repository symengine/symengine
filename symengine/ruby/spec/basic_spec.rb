require 'spec_helper'

describe SymEngine do
  before :each do
  end

  describe SymEngine::Basic do
    before :each do
    end

    describe '.new' do
      context 'with no arguments' do
        it 'returns a Basic object' do
          basic = SymEngine::Basic.new
          expect(basic).to be_an_instance_of SymEngine::Basic
        end
      end
    end

    describe 'binary operations' do
      before :each do
        @a = SymEngine::Symbol.new('x')
        @b = SymEngine::Symbol.new('y')
      end
      describe '#+' do
        context 'with another initialised Basic object as argument' do
          it 'returns a initialised Basic object that is result of
                        self added to the argument' do
            c = @a + @b
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c.to_s).to eql('x + y')
          end
        end
      end
      describe '#-' do
        context 'with another initialised Basic object as argument' do
          it 'returns a initialised Basic object that is result of
                        argument subtracted from self' do
            c = @a - @b
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c.to_s).to eql('x - y')
          end
        end
      end
      describe '#*' do
        context 'with another initialised Basic object as argument' do
          it 'returns a initialised Basic object that is result of
                        self multiplied by the argument' do
            c = @a * @b
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c.to_s).to eql('x*y')
          end
        end
      end
      describe '#/' do
        context 'with another initialised Basic object as argument' do
          it 'returns a initialised Basic object that is result of
                        self divided by the argument' do
            c = @a / @b
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c.to_s).to eql('x/y')
          end
        end
      end
      describe '#**' do
        context 'with another initialised Basic object as argument' do
          it 'returns a initialised Basic object that is result of
                        self raised to the power of argument' do
            c = @a**@b
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c.to_s).to eql('x**y')
          end
        end
      end
      describe '#diff' do
        context 'with another initialised Basic object as argument' do
          it 'differentiates self with respect to the argument
              and returns the result' do
            a = @a**3
            c = a.diff(@a)
            expect(c).to be_an_instance_of SymEngine::Basic
            expect(c).to eq(3 * @a**2)
            expect(a.diff(2)).to be_nil
          end
        end
      end
      describe '#==' do
        context 'with another initialised Basic object as argument' do
          it 'returns true if they are the same expression
                        false if not' do
            a = SymEngine::Symbol.new('x')
            b = SymEngine::Symbol.new('y')
            c = ((a * b) == (@a * @b))
            expect(c).to be true
          end
        end
      end
      describe '#eql?' do
        context 'with another initialised Basic object as argument' do
          it 'returns true if they are the same expression
                        false if not' do
            a = SymEngine::Symbol.new('x')
            b = SymEngine::Symbol.new('y')
            c = ((a * b).eql?(@a * @b))
            expect(c).to be true
          end
        end
      end
      describe '#!=' do
        context 'with another initialised Basic object as argument' do
          it 'returns true is they are not the same expression
                        false if they are' do |_variable|
            a = SymEngine::Symbol.new('x')
            b = SymEngine::Symbol.new('y')
            c = ((a * b) != (@a * @b))
            expect(c).to be false
          end
        end
      end
    end

    describe 'unary operations' do
      before :each do
        @x = SymEngine::Symbol.new('a')
      end
      describe '#-@' do
        context "doesn't take any argument" do
          it 'returns the negation of self' do
            p = -@x
            expect(p).to be_an_instance_of SymEngine::Basic
            expect(p.to_s).to eql('-a')
          end
        end
      end
    end

    describe '#args' do
      context 'with nothing as argument' do
        it 'returns array of arguments' do
          x = SymEngine::Symbol.new('x')
          y = SymEngine::Symbol.new('y')
          z = SymEngine::Symbol.new('z')
          e = (x**y + z)
          f = e.args
          expect(f).to be_an_instance_of Array
          expect(f.length).to be 2
          expect(f.to_set).to eql([x**y, z].to_set)
        end
      end
    end

    describe '#free_symbols' do
      context 'with nothing as argument' do
        it 'returns the set of symbols' do
          x = SymEngine::Symbol.new('x')
          y = SymEngine::Symbol.new('y')
          z = SymEngine::Symbol.new('z')
          e = (x**y / z)
          f = e.free_symbols
          expect(f).to be_an_instance_of Set
          expect(f.length).to be 3
          expect(f).to eql([x, y, z].to_set)
        end
      end
    end

    describe '#expand' do
      context 'with nothing as argument' do
        it 'returns the expanded form' do
          x = SymEngine::Symbol.new('x')
          y = SymEngine::Symbol.new('y')
          z = SymEngine::Symbol.new('z')
          e = (x + y + z) * (x + y + z)
          f = e.expand
          expect(e.to_s).to eql('(x + y + z)**2')
          expect(f).to be_an_instance_of SymEngine::Basic
          expect(f.to_s).to eql('2*x*y + 2*x*z + 2*y*z + x**2 + y**2 + z**2')
          expect(e == f).to be false
        end
      end
    end

    describe '#subs' do
      before :each do
        @x = SymEngine::Symbol.new('x')
        @y = SymEngine::Symbol.new('y')
        @z = SymEngine::Symbol.new('z')
        @e = @x + @y + @z
      end

      context 'with two basic objects as argument' do
        xit 'returns the expression with first
            substituted with second in self' do
          expect(@e.subs(@x, @y)).to eql(2 * @y + @z)
        end
      end

      context 'with a Hash as argument' do
        xit 'returns the expression with
            each key subtituted with its mapped value' do
          expect(@e.subs(@x => @y, @z => @y)).to eql(3 * @y)
        end
      end
    end
  end
end
