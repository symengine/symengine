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
          expect do
            SymEngine::Integer.new(-12_345_678_912_345_678_912)
          end.to raise_error(RangeError)
        end
      end
    end
  end
end
