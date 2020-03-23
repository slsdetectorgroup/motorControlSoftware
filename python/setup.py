from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import os

__version__ = os.environ.get('GIT_DESCRIBE_TAG', 'developer')

setup(
    name='slsdetbox',
    version = __version__,
    author='Erik Frojdh',
    author_email='erik.frojdh@psi.ch',
    url='https://github.com/slsdetectorgroup/motorControl',
    description='Xray box control',
    long_description='',
    packages=find_packages(exclude=['contrib', 'docs', 'tests']),
    zip_safe=False,
)