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
  end
end
