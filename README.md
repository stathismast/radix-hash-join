# Project_Databases

sdi1500039
sdi1500092
sdi1500147

TODO:
- Refactor reOrderPredicates()
- Update reOrderPredicates() to prioritize joins between relations that are already in the intermediate results
- Test functionality with testHarness.cpp
- Support queries that don't use a filter
- Update reOrderPredicates() to calculate the total number of intermediate results needed to execute a given query
- Update reOrderPredicates() to produce an efficient order of operations for queries that require more than one interemediate result structure. Maybe prioritize operations that lead to the merging of two IRs.



