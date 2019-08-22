After a long time, a finally decided to update this excellent tool.
This is the first release (1.0.2) from me, Thorsten. It fixes a few bugs,
but also has undergone some rewrite. Major changes include:

- all codecs are now compiled as SLBs
- Third-party-libraries like TIFF, PNG etc are also loaded
  as shared libraries
- The PDF module has also been separated, and is only loaded
  when you actually open a PDF document

As before, you don't need to install anything; the codecs are
looked up in the applications folder "codecs" directory.
However if you want to use the standard libaries (pnglib etc.)
also for other projects, you may want to copy them to some directory
of your SLBPATH environment variable (usually C:\mint\slb when running
MiNT, or C:\gemsys\magic\xtension when running MagiC). Note that
the libraries also should work with SingleTOS.

You can also copy the codecs folder to that same place (including the folder
itself). VISION for example may be able to make use of them.

See also the NEWS.TXT file for a list of recent changes.

Thorsten Otto
August, 2019
