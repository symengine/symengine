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
                @a = SymEngine::Basic.new
                @b = SymEngine::Basic.new
            end
            describe '#+' do
                context 'with another Basic object as argument' do
                    xit 'returns a Basic object that is result of
                        self added to the argument' do
                    end
                end
            end
            describe '#-' do
                context 'with another Basic object as argument' do
                    xit 'returns a Basic object that is result of
                        argument subtracted from self' do
                    end
                end
            end
            describe '#*' do
                context 'with another Basic object as argument' do
                    xit 'returns a Basic object that is result of
                        self multiplied by the argument' do
                    end
                end
            end
            describe '#/' do
                context 'with another Basic object as argument' do
                    xit 'returns a Basic object that is result of
                        self divided by the argument' do
                    end
                end
            end
            describe '#**' do
                context 'with another Basic object as argument' do
                    xit 'returns a Basic object that is result of
                        self raised to the power of argument' do
                    end
                end
            end
        end

        describe 'unary operations' do
            describe '#-@' do
                context "doesn't take any argument" do
                    xit 'returns the negation of self' do
                    end
                end
            end
        end
    end
end
