Gem::Specification.new do |gem|
	gem.name = 'symengine'
	gem.version = '0.0.0.pre'
	gem.summary = "Ruby interface for SymEngine, a fast C++ library for symbolic manipulation"
	gem.description = "This gem provides a Ruby interface for SymEngine, a fast C++ library for symbolic manipulation, so that the you can use the features of SymEngine in ruby."
	gem.authors = ['Abinash Meher']
	gem.email = ["abinashdakshana999@gmail.com"]
	gem.extensions = ['ext/symengine/extconf.rb']
	gem.files = Dir["lib/**/*.rb", "bin/*", "LICENSE", "*.md"]
	gem.require_paths = ["lib"]
	gem.license = 'MIT'
end
