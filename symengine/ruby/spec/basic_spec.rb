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
                @a = SymEngine::Symbol.new("x")
                @b = SymEngine::Symbol.new("y")
            end
            describe '#+' do
                context 'with another initialised Basic object as argument' do
                    it 'returns a initialised Basic object that is result of
                        self added to the argument' do
                        c = @a+@b
                        expect(c).to be_an_instance_of SymEngine::Symbol
                        expect(c.to_s).to eql("x + y")
                    end
                end
            end
            describe '#-' do
                context 'with another initialised Basic object as argument' do
                    it 'returns a initialised Basic object that is result of
                        argument subtracted from self' do
                        c = @a-@b
                        expect(c).to be_an_instance_of SymEngine::Symbol
                        expect(c.to_s).to eql("x - y")
                    end
                end
            end
            describe '#*' do
                context 'with another initialised Basic object as argument' do
                    it 'returns a initialised Basic object that is result of
                        self multiplied by the argument' do
                        c = @a*@b
                        expect(c).to be_an_instance_of SymEngine::Symbol
                        expect(c.to_s).to eql("x*y")
                    end
                end
            end
            describe '#/' do
                context 'with another initialised Basic object as argument' do
                    it 'returns a initialised Basic object that is result of
                        self divided by the argument' do
                        c = @a/@b
                        expect(c).to be_an_instance_of SymEngine::Symbol
                        expect(c.to_s).to eql("x/y")
                    end
                end
            end
            describe '#**' do
                context 'with another initialised Basic object as argument' do
                    it 'returns a initialised Basic object that is result of
                        self raised to the power of argument' do
                        c = @a**@b
                        expect(c).to be_an_instance_of SymEngine::Symbol
                        expect(c.to_s).to eql("x**y")
                    end
                end
            end
        end

        describe 'unary operations' do
            before :each do
                @x = SymEngine::Symbol.new("a")
            end
            describe '#-@' do
                context "doesn't take any argument" do
                    it 'returns the negation of self' do
                        p = -@x
                        expect(p).to be_an_instance_of SymEngine::Symbol
                        expect(p.to_s).to eql("-a")
                    end
                end
            end
        end

        describe '#expand' do
            context 'with nothing as argument' do
                it 'returns the expanded form' do
                    x = SymEngine::Symbol.new("x")
                    y = SymEngine::Symbol.new("y")
                    z = SymEngine::Symbol.new("z")
                    e = (x+y+z)*(x+y+z)
                    f = e.expand()
                    expect(e.to_s).to eql("(x + y + z)**2")
                    expect(f).to be_an_instance_of SymEngine::Symbol
                    expect(f.to_s).to eql("2*x*y + 2*x*z + 2*y*z + x**2 + y**2 + z**2")
                    expect(e==f).to be false
                end
            end
        end
    end
end
