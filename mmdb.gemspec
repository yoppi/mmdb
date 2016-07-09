Gem::Specification.new do |spec|
  spec.name          = "mmdb"
  spec.version       = "0.1.0"
  spec.authors       = ["yoppi"]
  spec.email         = ["y.hirokazu@gmail.com"]

  spec.summary       = %q{MaxMindDB client for Ruby}
  spec.description   = %q{Fast looking-up IP address from MaxMindDB(GeoIP2 format)}
  spec.homepage      = "https://github.com/yoppi/mmdb"
  spec.license       = "MIT"

  spec.extensions    = Dir["ext/**/extconf.rb"]
  spec.files         = Dir["ext/**/*.{rb,c,h}"]

  spec.add_development_dependency "bundler", "~> 1.12"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "minitest", "~> 5.0"
end
