from distutils.core import setup
from Cython.Build import cythonize
setup(ext_modules = cythonize(["src/curve.py","src/ecdhpsi.py", "src/ecdhpsi_base.py",],language_level = "3"))