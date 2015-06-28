if defined?(IRuby)
  module SymEngine
    class Basic
      def to_iruby
        ['text/html', self.to_s] unless self.instance_of? Basic
      end
    end
  end
end
