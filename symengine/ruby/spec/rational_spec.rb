require 'spec_helper'

describe SymEngine do
  describe SymEngine::Rational do
    describe '.new' do
      context 'with a Ruby Rational object as input' do
        it 'returns an instance of SymEngine::Rational class' do
          a = Rational('2/3')
          a = SymEngine::Rational.new(a)
          expect(a).to be_an_instance_of SymEngine::Rational
          expect(a.to_s).to eq('2/3')
        end
      end
    end

    describe 'coercion tests' do
      before :each do
        @a = SymEngine::Symbol.new('x')
        @b = Rational(3, 5)
      end

      context 'using a ruby Rational as the second operand' do
        it 'succeeds with commutative operations' do
          c = @a * @b
          expect(c).to be_an_instance_of SymEngine::Basic
          expect(c).to eq(SymEngine::Rational.new(@b) * @a)
        end

        it 'succeeds with non commutative operations' do
          c = @a / @b
          expect(c).to be_an_instance_of SymEngine::Basic
          expect(c).to eq(@a / SymEngine::Rational.new(@b))
        end
      end

      context 'using a ruby Rational as the first operand' do
        it 'succeeds with commutative operations' do
          c = @b * @a
          expect(c).to be_an_instance_of SymEngine::Basic
          expect(c).to eq(@a * SymEngine::Rational.new(@b))
        end

        it 'succeeds with non commutative operations' do
          c = @b / @a
          expect(c).to be_an_instance_of SymEngine::Basic
          expect(c).to eq(SymEngine::Rational.new(@b) / @a)
        end
      end
    end
  end
end
