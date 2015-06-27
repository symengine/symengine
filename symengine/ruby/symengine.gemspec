Gem::Specification.new do |gem|
    gem.name = 'symengine'
    gem.version = '0.0.0'
    gem.summary = "Ruby interface for SymEngine, a fast C++ library for symbolic manipulation"
    gem.description = "This gem provides a Ruby interface for SymEngine, a fast C++ library for symbolic manipulation, so that the you can use the features of SymEngine in ruby."
    gem.authors = ['Abinash Meher']
    gem.email = ["abinashdakshana999@gmail.com"]
    gem.homepage = 'https://github.com/sympy/symengine'
    gem.files = Dir["lib/**/*", "bin/*", "LICENSE", "*.md"]
    gem.require_paths = ["lib"]
    gem.license = 'MIT'

    gem.add_development_dependency 'bundler', '~> 1.7'
    gem.add_development_dependency 'rspec', '~> 3.0'
    gem.add_development_dependency 'test-unit', '~> 3.1'
    gem.add_development_dependency 'rdoc', '~> 4.0'
end
