#ifndef TREE_SITTER_FSHARP_H_
#define TREE_SITTER_FSHARP_H_

typedef struct TSLanguage TSLanguage;

#ifdef __cplusplus
extern "C" {
#endif

extern TSLanguage *tree_sitter_fsharp();
extern TSLanguage *tree_sitter_fsharp_signature();

#ifdef __cplusplus
}
#endif

#endif // TREE_SITTER_FSHARP_H_
