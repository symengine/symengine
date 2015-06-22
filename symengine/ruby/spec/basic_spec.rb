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
    end
end
