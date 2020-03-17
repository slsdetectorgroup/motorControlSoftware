from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import os

def get_version():
    with open('../conda-recepie/meta.yaml', 'r') as f:
        l = f.readline()
        return l.split("\"")[-2]

__version__ = get_version()

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