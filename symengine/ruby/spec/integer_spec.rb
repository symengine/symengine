require 'spec_helper'

describe SymEngine do
  describe SymEngine::Integer do
    describe '#new' do
      context 'with Integer as argument' do
        it 'gives a new SymEngine::Integer instance' do
          a = SymEngine::Integer.new(123)
          b = SymEngine::Integer.new(-123)
          expect(a).to be_an_instance_of SymEngine::Integer
          expect(b).to be_an_instance_of SymEngine::Integer
          expect(a.to_s).to eq('123')
          expect(b.to_s).to eq('-123')
          c = SymEngine::Integer.new(12_345_678_912_345_678_912)
          expect(c.to_s).to eq('12345678912345678912')
          c = SymEngine::Integer.new(-12_345_678_912_345_678_912)
          expect(c.to_s).to eq('-12345678912345678912')
        end
      end
    end

    describe 'coercion tests' do
      before :each do
        @a = SymEngine::Symbol.new('x')
      end

      context 'using a ruby integer as the second operand' do
        it 'succeeds with commutative operations' do
          b = @a * 2
          expect(b).to be_an_instance_of SymEngine::Basic
          expect(b).to eq(SymEngine::Integer.new(2) * @a)
        end

        it 'succeeds with non commutative operations' do
          b = @a / 2
          expect(b).to be_an_instance_of SymEngine::Basic
          expect(b).to eq(@a / SymEngine::Integer.new(2))
        end
      end

      context 'using a ruby integer as the first operand' do
        it 'succeeds with commutative operations' do
          b = 2 * @a
          expect(b).to be_an_instance_of SymEngine::Basic
          expect(b).to eq(@a * SymEngine::Integer.new(2))
        end

        it 'succeeds with non commutative operations' do
          b = 2 / @a
          expect(b).to be_an_instance_of SymEngine::Basic
          expect(b).to eq(SymEngine::Integer.new(2) / @a)
        end
      end
    end
  end
end
