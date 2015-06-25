require 'spec_helper'

describe SymEngine do
    before :each do
    end

    describe SymEngine::Symbol do
        before :each do
        end

        describe '.new' do
            context 'with a string as an argument' do
                it 'returns a Symbol object' do
                    symbol = SymEngine::Symbol.new('x')
                    expect(symbol).to be_an_instance_of SymEngine::Symbol
                end
            end
        end
    end
end
