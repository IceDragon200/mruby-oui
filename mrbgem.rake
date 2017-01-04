MRuby::Gem::Specification.new('mruby-oui') do |spec|
  spec.license = 'MIT'
  spec.authors = ['Corey Powell']
  spec.version = '0.0.0'
  spec.summary = 'oui bindings'
  spec.description = 'A oui binding for mruby'
  spec.homepage = 'https://github.com/IceDragon200/mruby-oui'
  # compiler config
  spec.cc.include_paths << ["#{build.root}/src"]
  # I think we inherited the parent flags, might as well flatten the array to
  # be sure we're working with Array<String> and not Array<String, Array<String>>
  spec.cc.flags = spec.cc.flags.flatten
  # the blendish code is written in C99 style,
  # the declaration-after-statement warning will cause it to flip out, since
  # I treat all warnings as errors...
  spec.cc.flags.reject! { |s| s.include?('-Wdeclaration-after-statement') }
end
