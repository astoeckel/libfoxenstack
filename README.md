# libfoxenstack ― Library for switching user-space stacks

This library provides a simple interface for executing a function in the
context of a different stack. This is particularly useful for user-space
threading libraries. For now, supported platforms are Linux (i386, x86_64, ARM).
Patches for other platforms are welcome.


## FAQ about the *Foxen* series of C libraries

**Q: What's with the name?**

**A:** [*Foxen*](http://kingkiller.wikia.com/wiki/Foxen) is a mysterious glowing object guiding Auri through the catacumbal “Underthing”. The *Foxen* software libraries are similar in key aspects: mysterious and catacumbal. Probably less useful than an eternal sympathy lamp though.

**Q: What is the purpose and goal of these libraries?**

**A:** The *Foxen* libraries are extremely small C libraries that rely on the [Meson](https://mesonbuild.com/) build system for dependency management. One common feature is that the libraries do not use [heap memory allocations](https://github.com/astoeckel/libfoxenmem).

**Q: Why?**

**A:** Excellent question! The author mainly created these libraries because he grew tired of copying his own source code files between projects all the time.

**Q: Would you recommend to use these libraries in my project?**

**A:** That depends. Some of the code is fairly specialized according to my own needs and might not be intended to be general. If what you are going to use these libraries for aligns with their original purpose, then sure, go ahead. Otherwise, I'd probably advise against using these libraries, and as explained below, I'm not super open to expanding their scope.

**Q: Can you licence these libraries under a something more permissive than AGPLv3?**

**A:** Maybe, if you ask nicely. I'm not a fan of giving my work away “for free” (i.e., allowing inclusion of my code in commercial or otherwise proprietary software) without getting something back (in particular public access to the source code of the things other people built with it). That being said, some of the `foxen` libraries may be too trivial to warrant the use of a strong copyleft licence. Correspondingly, I might reconsider this decision for individual libraries. See “[Why you shouldn't use the Lesser GPL for your next library](https://www.gnu.org/licenses/why-not-lgpl.en.html)” for more info.

**Q: Can I contribute?**

**A:** Sure! Feel free to open an issue or a PR. However, be warned that since I've mainly developed these libraries for use in my own stuff, I might be a little picky about what I'm going to include and what not.

## Licence

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.
