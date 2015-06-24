Steps to compile the extensions
===============================
* Install bundler by `gem install bundler`
* Change directory to src/ruby, `cd symengine/ruby`
* Execute `bundle install` this will install the gems required
* Change to root directory, `cd ../..`
* Build symengine with Ruby by adding the flag, `cmake -DWITH_RUBY=yes .`
* Then cd to symengine/ruby again, `cd symengine/ruby`
* Build the gem, `gem build symengine.gemspec`
* Install the gem, `gem install symengine-0.0.0.pre.gem`

Using the extensions
====================
SymEngine is a module in the extensions, and the classes are a part of it. So
first you fire up the interpreter and load the file
``` ruby
~$ irb
2.2.0 :001 > require 'symengine'
 => true

```
Go ahead and try a function
``` ruby
2.2.0 :002 > SymEngine.ascii_art

 _____           _____         _
|   __|_ _ _____|   __|___ ___|_|___ ___
|__   | | |     |   __|   | . | |   | -_|
|_____|_  |_|_|_|_____|_|_|_  |_|_|_|___|
      |___|               |___|
 => nil
```
or create a variable
``` ruby
2.2.0 :003 > basic = SymEngine::Basic.new
 => #<SymEngine::Basic:0x000000027075b8>
```
