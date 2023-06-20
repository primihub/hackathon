/*
Copyright IBM Corp. All Rights Reserved.

SPDX-License-Identifier: Apache-2.0
*/

package etcdraft

import (
	"io/ioutil"
	"os"
	"path"
	"path/filepath"
	"sort"
	"strings"
	"testing"

	"github.com/hyperledger/fabric/common/flogging"
	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
	"go.etcd.io/etcd/pkg/fileutil"
	"go.etcd.io/etcd/raft"
	"go.etcd.io/etcd/raft/raftpb"
	"go.etcd.io/etcd/wal"
	"go.uber.org/zap"
)

var (
	err    error
	logger *flogging.FabricLogger

	dataDir, walDir, snapDir string

	ram   *raft.MemoryStorage
	store *RaftStorage
)

func setup(t *testing.T) {
	logger = flogging.NewFabricLogger(zap.NewExample())
	ram = raft.NewMemoryStorage()
	dataDir, err = ioutil.TempDir("", "etcdraft-")
	assert.NoError(t, err)
	walDir, snapDir = path.Join(dataDir, "wal"), path.Join(dataDir, "snapshot")
	store, err = CreateStorage(logger, walDir, snapDir, ram)
	assert.NoError(t, err)
}

func clean(t *testing.T) {
	err = store.Close()
	assert.NoError(t, err)
	err = os.RemoveAll(dataDir)
	assert.NoError(t, err)
}

func fileCount(files []string, suffix string) (c int) {
	for _, f := range files {
		if strings.HasSuffix(f, suffix) {
			c++
		}
	}
	return
}

func assertFileCount(t *testing.T, wal, snap int) {
	files, err := fileutil.ReadDir(walDir)
	assert.NoError(t, err)
	assert.Equal(t, wal, fileCount(files, ".wal"), "WAL file count mismatch")

	files, err = fileutil.ReadDir(snapDir)
	assert.NoError(t, err)
	assert.Equal(t, snap, fileCount(files, ".snap"), "Snap file count mismatch")
}

func TestOpenWAL(t *testing.T) {
	t.Run("Last WAL file is broken", func(t *testing.T) {
		setup(t)
		defer clean(t)

		// create 10 new wal files
		for i := 0; i < 10; i++ {
			store.Store(
				[]raftpb.Entry{{Index: uint64(i), Data: make([]byte, 10)}},
				raftpb.HardState{},
				raftpb.Snapshot{},
			)
		}
		assertFileCount(t, 1, 0)
		lasti, _ := store.ram.LastIndex() // it never returns err

		// close current storage
		err = store.Close()
		assert.NoError(t, err)

		// truncate wal file
		w := func() string {
			files, err := fileutil.ReadDir(walDir)
			assert.NoError(t, err)
			for _, f := range files {
				if strings.HasSuffix(f, ".wal") {
					return path.Join(walDir, f)
				}
			}
			t.FailNow()
			return ""
		}()
		err = os.Truncate(w, 200)
		assert.NoError(t, err)

		// create new storage
		ram = raft.NewMemoryStorage()
		store, err = CreateStorage(logger, walDir, snapDir, ram)
		require.NoError(t, err)
		lastI, _ := store.ram.LastIndex()
		assert.True(t, lastI > 0)     // we are still able to read some entries
		assert.True(t, lasti > lastI) // but less than before because some are broken
	})
}

func TestTakeSnapshot(t *testing.T) {
	// To make this test more understandable, here's a list
	// of expected wal files:
	// (wal file name format: seq-index.wal, index is the first index in this file)
	//
	// 0000000000000000-0000000000000000.wal (this is created initially by etcd/wal)
	// 0000000000000001-0000000000000001.wal
	// 0000000000000002-0000000000000002.wal
	// 0000000000000003-0000000000000003.wal
	// 0000000000000004-0000000000000004.wal
	// 0000000000000005-0000000000000005.wal
	// 0000000000000006-0000000000000006.wal
	// 0000000000000007-0000000000000007.wal
	// 0000000000000008-0000000000000008.wal
	// 0000000000000009-0000000000000009.wal
	// 000000000000000a-000000000000000a.wal

	t.Run("Good", func(t *testing.T) {
		t.Run("MaxSnapshotFiles==1", func(t *testing.T) {
			backup := MaxSnapshotFiles
			MaxSnapshotFiles = 1
			defer func() { MaxSnapshotFiles = backup }()

			setup(t)
			defer clean(t)

			// set SegmentSizeBytes to a small value so that
			// every entry persisted to wal would result in
			// a new wal being created.
			oldSegmentSizeBytes := wal.SegmentSizeBytes
			wal.SegmentSizeBytes = 10
			defer func() {
				wal.SegmentSizeBytes = oldSegmentSizeBytes
			}()

			// create 10 new wal files
			for i := 0; i < 10; i++ {
				store.Store(
					[]raftpb.Entry{{Index: uint64(i), Data: make([]byte, 100)}},
					raftpb.HardState{},
					raftpb.Snapshot{},
				)
			}

			assertFileCount(t, 11, 0)

			store.TakeSnapshot(uint64(3), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			// Snapshot is taken at index 3, which releases lock up to 2 (excl.).
			// This results in wal files with index [0, 1] being purged (2 files)
			assertFileCount(t, 9, 1)

			store.TakeSnapshot(uint64(5), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			// Snapshot is taken at index 5, which releases lock up to 4 (excl.).
			// This results in wal files with index [2, 3] being purged (2 files)
			assertFileCount(t, 7, 1)

			t.Logf("Close the storage and create a new one based on existing files")

			err = store.Close()
			assert.NoError(t, err)
			ram := raft.NewMemoryStorage()
			store, err = CreateStorage(logger, walDir, snapDir, ram)
			assert.NoError(t, err)

			store.TakeSnapshot(uint64(7), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			// Snapshot is taken at index 7, which releases lock up to 6 (excl.).
			// This results in wal files with index [4, 5] being purged (2 file)
			assertFileCount(t, 5, 1)

			store.TakeSnapshot(uint64(9), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			// Snapshot is taken at index 9, which releases lock up to 8 (excl.).
			// This results in wal files with index [6, 7] being purged (2 file)
			assertFileCount(t, 3, 1)
		})

		t.Run("MaxSnapshotFiles==2", func(t *testing.T) {
			backup := MaxSnapshotFiles
			MaxSnapshotFiles = 2
			defer func() { MaxSnapshotFiles = backup }()

			setup(t)
			defer clean(t)

			// set SegmentSizeBytes to a small value so that
			// every entry persisted to wal would result in
			// a new wal being created.
			oldSegmentSizeBytes := wal.SegmentSizeBytes
			wal.SegmentSizeBytes = 10
			defer func() {
				wal.SegmentSizeBytes = oldSegmentSizeBytes
			}()

			// create 10 new wal files
			for i := 0; i < 10; i++ {
				store.Store(
					[]raftpb.Entry{{Index: uint64(i), Data: make([]byte, 100)}},
					raftpb.HardState{},
					raftpb.Snapshot{},
				)
			}

			assertFileCount(t, 11, 0)

			// Only one snapshot is taken, no wal pruning happened
			store.TakeSnapshot(uint64(3), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 11, 1)

			// Two snapshots at index 3, 5. And we keep one extra wal file prior to oldest snapshot.
			// So we should have pruned wal file with index [0, 1]
			store.TakeSnapshot(uint64(5), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 9, 2)

			t.Logf("Close the storage and create a new one based on existing files")

			err = store.Close()
			assert.NoError(t, err)
			ram := raft.NewMemoryStorage()
			store, err = CreateStorage(logger, walDir, snapDir, ram)
			assert.NoError(t, err)

			// Two snapshots at index 5, 7. And we keep one extra wal file prior to oldest snapshot.
			// So we should have pruned wal file with index [2, 3]
			store.TakeSnapshot(uint64(7), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 7, 2)

			// Two snapshots at index 7, 9. And we keep one extra wal file prior to oldest snapshot.
			// So we should have pruned wal file with index [4, 5]
			store.TakeSnapshot(uint64(9), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 5, 2)
		})
	})

	t.Run("Bad", func(t *testing.T) {

		t.Run("MaxSnapshotFiles==2", func(t *testing.T) {
			// If latest snapshot file is corrupted, storage should be able
			// to recover from an older one.

			backup := MaxSnapshotFiles
			MaxSnapshotFiles = 2
			defer func() { MaxSnapshotFiles = backup }()

			setup(t)
			defer clean(t)

			// set SegmentSizeBytes to a small value so that
			// every entry persisted to wal would result in
			// a new wal being created.
			oldSegmentSizeBytes := wal.SegmentSizeBytes
			wal.SegmentSizeBytes = 10
			defer func() {
				wal.SegmentSizeBytes = oldSegmentSizeBytes
			}()

			// create 10 new wal files
			for i := 0; i < 10; i++ {
				store.Store(
					[]raftpb.Entry{{Index: uint64(i), Data: make([]byte, 100)}},
					raftpb.HardState{},
					raftpb.Snapshot{},
				)
			}

			assertFileCount(t, 11, 0)

			// Only one snapshot is taken, no wal pruning happened
			store.TakeSnapshot(uint64(3), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 11, 1)

			// Two snapshots at index 3, 5. And we keep one extra wal file prior to oldest snapshot.
			// So we should have pruned wal file with index [0, 1]
			store.TakeSnapshot(uint64(5), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 9, 2)

			d, err := os.Open(snapDir)
			assert.NoError(t, err)
			defer d.Close()
			names, err := d.Readdirnames(-1)
			assert.NoError(t, err)
			sort.Sort(sort.Reverse(sort.StringSlice(names)))

			t.Logf("Corrupt latest snapshot file: %s", filepath.Join(snapDir, names[0]))
			f, err := os.OpenFile(filepath.Join(snapDir, names[0]), os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0666)
			assert.NoError(t, err)
			_, err = f.WriteString("Corrupted Snapshot")
			assert.NoError(t, err)
			f.Close()

			t.Logf("Close the storage and create a new one based on existing files")

			err = store.Close()
			assert.NoError(t, err)
			ram := raft.NewMemoryStorage()
			store, err = CreateStorage(logger, walDir, snapDir, ram)
			assert.NoError(t, err)

			// Corrupted snapshot file should've been renamed
			assertFileCount(t, 9, 1)

			files, err := fileutil.ReadDir(snapDir)
			assert.NoError(t, err)
			assert.Equal(t, 1, fileCount(files, ".broken"))

			store.TakeSnapshot(uint64(7), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 9, 2)

			store.TakeSnapshot(uint64(9), raftpb.ConfState{Nodes: []uint64{1}}, make([]byte, 10))
			assertFileCount(t, 5, 2)
		})
	})
}
