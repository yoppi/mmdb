# mmdb - MaxMindDB client

`mmdb` gem provide to fast look up ip address from MaxMindDB(in GeoIP2 format) database.

## Installation

`mmdb` has been depended to [libmaxminddb](https://github.com/maxmind/libmaxminddb).
Please install `libmaxminddb` with installation guide in README.md.

Then, add this line to your application's Gemfile:

```ruby
gem 'mmdb'
```

And then execute:

```
$ bundle
```

Or install it yourself as:

```
$ gem install mmdb
```

## Usage

```ruby
mmdb = MaxMindDB.new("path/to/GeoLite2-City.mmdb")
mmdb.lookup("118.240.137.106")
#=>
#{
# :city=>"Tokyo",
# :country=>"Japan",
# :country_code=>"JP",
# :continent=>"Asia",
# :latitude=>35.6502,
# :longitude=>139.6939,
# :postcode=>"153-0042"
#}
```

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/yoppi/mmdb.

## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).
